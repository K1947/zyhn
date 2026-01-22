#include "pwp_length_algorithm.h"
#include "property_editor_widget.h"
#include "result_shape.h" // 包含 ResultPath, ResultText
#include "logger_utils.h"
#include "roi_shape.h"    // 包含 RoiShape, RoiPolygon

#include <QPainterPath>
#include <memory> 
#include <exception> 
#include <QDebug> 

using namespace HalconCpp;

PwpLengthAlgorithm::PwpLengthAlgorithm() :
    prop_obj_(new ConfigurableObject()),
    init_param_status_(true)
{
    setupProperties();
}

PwpLengthAlgorithm::~PwpLengthAlgorithm()
{
    if (prop_obj_) {
        delete prop_obj_;
        prop_obj_ = nullptr;
    }
}

bool PwpLengthAlgorithm::initialize()
{
    return true;
}

QString PwpLengthAlgorithm::name() const
{
    return "pwp_length";
}

void PwpLengthAlgorithm::setupProperties()
{
    PropertyBuilder::create("选择图像源", "image_source")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"图像1", ""} })
        .defaultValue("图像1")
        .registerTo(prop_obj_);

    PropertyBuilder::create("二值化下限", "threshold_min")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(0)
        .registerTo(prop_obj_);

    PropertyBuilder::create("二值化上限", "threshold_max")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(59)
        .registerTo(prop_obj_);

    PropertyBuilder::create("开运算半径", "opening_radius")
        .category("预处理")
        .type(QMetaType::Double)
        .defaultValue(13)
        .registerTo(prop_obj_);

    PropertyBuilder::create("最小区域面积", "min_area")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(4000)
        .registerTo(prop_obj_);

    PropertyBuilder::create("最小骨架长度", "min_skel_len")
        .category("骨架提取")
        .type(QMetaType::Double)
        .defaultValue(50.0)
        .registerTo(prop_obj_);

    PropertyBuilder::create("最大连接间隙", "max_gap")
        .category("轮廓连接")
        .type(QMetaType::Double)
        .defaultValue(210.0)
        .registerTo(prop_obj_);

    prop_obj_->registerBtn(1, "测试运行");
}

ConfigurableObject* PwpLengthAlgorithm::getPropertyObj()
{
    return prop_obj_;
}

void PwpLengthAlgorithm::setParameters(const QJsonObject& p)
{
    prop_obj_->setPropetyFromJson(p);
    init_param_status_ = true;
}

void PwpLengthAlgorithm::setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p)
{
    for (auto it = p.begin(); it != p.end(); it++) {
        prop_obj_->setPropertyEnumValues(it.key(), it.value());
    }
}

AlgorithmResult PwpLengthAlgorithm::run(const AlgorithmContext& context)
{
    return test(context.input);
}

// 辅助：将 Halcon Region 转为 ResultPath (调试用)
void addRegionToResult(AlgorithmResult& result, const HObject& region, const QColor& color, int limit = 50) {
    if (!region.IsInitialized()) return;

    HTuple count;
    CountObj(region, &count);
    if (count.I() == 0) return;

    int drawLimit = (count.I() > limit) ? limit : count.I();

    for (int i = 1; i <= drawLimit; ++i) {
        HObject single;
        SelectObj(region, &single, i);
        HTuple rows, cols;
        try {
            // 使用 GetRegionPolygon 获取多边形边界
            GetRegionPolygon(single, 5.0, &rows, &cols);
        }
        catch (...) { continue; }

        if (rows.Length() > 0) {
            QPainterPath path;
            path.moveTo(cols[0].D(), rows[0].D());
            for (int k = 1; k < rows.Length(); ++k) {
                path.lineTo(cols[k].D(), rows[k].D());
            }
            path.closeSubpath();
            auto pathShape = std::make_shared<ResultPath>(path, color);
            result.addResultShape(pathShape);
        }
    }
}

AlgorithmResult PwpLengthAlgorithm::test(const AlgorithmInput& input)
{
    AlgorithmResult result;

    if (!init_param_status_) {
        result.setCode(-3);
        result.setMsg("初始化参数失败");
        return result;
    }

    try {
        // 1. 获取图像
        if (input.imageCount() <= 0) {
            result.setCode(-1);
            result.setMsg("没有输入图像");
            return result;
        }
        HObject hImg = input.getImageAsHObject(0);
        if (!hImg.IsInitialized()) {
            result.setCode(-1);
            result.setMsg("图像无效");
            return result;
        }

        // 2. 获取参数
        int thMin = prop_obj_->propValue("threshold_min").toInt();
        int thMax = prop_obj_->propValue("threshold_max").toInt();
        double openRadius = prop_obj_->propValue("opening_radius").toDouble();
        int minArea = prop_obj_->propValue("min_area").toInt();
        double minSkelLen = prop_obj_->propValue("min_skel_len").toDouble();
        double maxGap = prop_obj_->propValue("max_gap").toDouble();

        // 3. ROI 解析 (XLD 填充模式)
        HObject ho_ROI_Search;
        GenEmptyObj(&ho_ROI_Search);
        bool has_valid_roi = false;

        if (input.roiShapes().size() > 0) {
            for (const auto& shape : input.roiShapes()) {
                if (!shape) continue;

                HObject ho_TempRegion;
                bool shape_converted = false;

                // A. 多边形
                if (shape->type() == RoiShape::Polygon) {
                    auto polyShape = std::dynamic_pointer_cast<RoiPolygon>(shape);
                    if (polyShape) {
                        QVector<QPointF> pts = polyShape->getPts();
                        if (pts.size() >= 3) {
                            HTuple hv_Rows, hv_Cols;
                            for (const auto& pt : pts) {
                                hv_Rows.Append(pt.y()); // Row
                                hv_Cols.Append(pt.x()); // Col
                            }

                            // 轮廓 -> 填充区域
                            HObject ho_Contour;
                            GenContourPolygonXld(&ho_Contour, hv_Rows, hv_Cols);
                            GenRegionContourXld(ho_Contour, &ho_TempRegion, "filled");

                            shape_converted = true;
                        }
                    }
                }
                // B. 矩形
                else if (shape->type() == RoiShape::Rectangle) {
                    QRectF r = shape->boundingRect();
                    GenRectangle1(&ho_TempRegion, r.top(), r.left(), r.bottom(), r.right());
                    shape_converted = true;
                }
                // C. 圆形
                else if (shape->type() == RoiShape::Circle) {
                    QRectF r = shape->boundingRect();
                    double radius = r.width() / 2.0;
                    GenCircle(&ho_TempRegion, r.center().y(), r.center().x(), radius);
                    shape_converted = true;
                }

                if (shape_converted) {
                    if (!has_valid_roi) {
                        ho_ROI_Search = ho_TempRegion;
                        has_valid_roi = true;
                    }
                    else {
                        Union2(ho_ROI_Search, ho_TempRegion, &ho_ROI_Search);
                    }
                }
            }
        }

        if (!has_valid_roi) {
            GetDomain(hImg, &ho_ROI_Search);
        }
        else {
            // 调试：绘制蓝色ROI
            addRegionToResult(result, ho_ROI_Search, Qt::blue);
        }

        // 4. 执行 Halcon 算法
        HObject ho_R, ho_ImageReduced;
        HObject ho_ImageGauss, ho_ImageScaled, ho_RegionDark;
        HObject ho_ConnectedRegions, ho_RegionClean;
        HObject ho_BigRegions, ho_RegionUnion, ho_RegionClosed, ho_RegionFilled;
        HObject ho_RegionSmooth, ho_RegionFinal, ho_SkeletonRegion;
        HObject ho_SkeletonContours, ho_ContoursSplit, ho_MainContours;
        HObject ho_UnionContours, ho_MajorSegments, ho_FinalUnion;
        HObject ho_FinalLine, ho_SmoothedLine;
        HTuple hv_LengthPixels;

        AccessChannel(hImg, &ho_R, 1);
        ReduceDomain(ho_R, ho_ROI_Search, &ho_ImageReduced);

        GaussFilter(ho_ImageReduced, &ho_ImageGauss, 3.0);
        ScaleImageMax(ho_ImageGauss, &ho_ImageScaled);

        Threshold(ho_ImageScaled, &ho_RegionDark, thMin, thMax);

        // 调试：绘制二值化结果（红色）
        addRegionToResult(result, ho_RegionDark, Qt::red);

        // 检查点 1
        HTuple areaDebug;
        AreaCenter(ho_RegionDark, &areaDebug, 0, 0);
        if (areaDebug.D() == 0) {
            result.setMsg(QString("NG: 二值化后为空 (阈值 %1-%2 不合适)").arg(thMin).arg(thMax));
            result.setResultType(ResultType::OK);
            return result;
        }

        Connection(ho_RegionDark, &ho_ConnectedRegions);

        if (openRadius > 0.001) {
            OpeningCircle(ho_ConnectedRegions, &ho_RegionClean, openRadius);
        }
        else {
            ho_RegionClean = ho_ConnectedRegions;
        }

        SelectShape(ho_RegionClean, &ho_BigRegions, "area", "and", minArea, 99999999);

        // 检查点 2
        CountObj(ho_BigRegions, &areaDebug);
        if (areaDebug.I() == 0) {
            result.setMsg(QString("NG: 筛选后为空 (最大面积 < %1)").arg(minArea));
            result.setResultType(ResultType::OK);
            return result;
        }

        Union1(ho_BigRegions, &ho_RegionUnion);
        ClosingCircle(ho_RegionUnion, &ho_RegionClosed, 10.5);
        FillUp(ho_RegionClosed, &ho_RegionFilled);
        OpeningCircle(ho_RegionFilled, &ho_RegionSmooth, 5.5);
        ErosionCircle(ho_RegionSmooth, &ho_RegionFinal, 1.5);

        Skeleton(ho_RegionFinal, &ho_SkeletonRegion);
        GenContoursSkeletonXld(ho_SkeletonRegion, &ho_SkeletonContours, 1, "filter");
        SegmentContoursXld(ho_SkeletonContours, &ho_ContoursSplit, "lines_circles", 5, 4, 2);

        SelectContoursXld(ho_ContoursSplit, &ho_MainContours, "contour_length", minSkelLen, 9999999, -0.5, 0.5);

        HTuple skelCount;
        CountObj(ho_MainContours, &skelCount);
        if (skelCount.I() == 0) {
            result.setMsg("NG: 骨架提取后为空");
            result.setResultType(ResultType::OK);
            return result;
        }

        UnionCollinearContoursXld(ho_MainContours, &ho_UnionContours, maxGap, 2.0, 20, 0.7, "attr_keep");

        double secondaryLen = (minSkelLen * 2.0 > 100.0) ? 100.0 : (minSkelLen * 2.0);
        SelectContoursXld(ho_UnionContours, &ho_MajorSegments, "contour_length", secondaryLen, 9999999, -0.5, 0.5);

        UnionAdjacentContoursXld(ho_MajorSegments, &ho_FinalUnion, 500, 1, "attr_keep");

        double finalLen = (minSkelLen * 3.0 > 300.0) ? 300.0 : (minSkelLen * 3.0);
        SelectContoursXld(ho_FinalUnion, &ho_FinalLine, "contour_length", finalLen, 9999999, -0.5, 0.5);

        SmoothContoursXld(ho_FinalLine, &ho_SmoothedLine, 31);
        LengthXld(ho_SmoothedLine, &hv_LengthPixels);

        // 5. 结果转换与显示
        double totalLen = 0.0;
        if (hv_LengthPixels.Length() > 0) {
            for (int i = 0; i < hv_LengthPixels.Length(); i++) {
                totalLen += hv_LengthPixels[i].D();
            }
        }

        if (totalLen > 0) {
            // 画出所有检测到的绿色曲线
            HTuple count;
            CountObj(ho_SmoothedLine, &count);
            for (int i = 1; i <= count.I(); ++i) {
                HObject singleObj;
                SelectObj(ho_SmoothedLine, &singleObj, i);
                HTuple rows, cols;
                GetContourXld(singleObj, &rows, &cols);

                if (rows.Length() > 0) {
                    QPainterPath path;
                    path.moveTo(cols[0].D(), rows[0].D());
                    for (int k = 1; k < rows.Length(); ++k) {
                        path.lineTo(cols[k].D(), rows[k].D());
                    }
                    auto pathShape = std::make_shared<ResultPath>(path, Qt::green);
                    result.addResultShape(pathShape);
                }
            }

            // 【核心修改】：在图像左上角显示总长度
            auto text = std::make_shared<ResultText>();
            text->text_ = QString("Total Length: %1 px").arg(totalLen, 0, 'f', 2);
            text->text_pos_ = QPointF(20, 50);
            text->font_size_ = 40; // 字体
            text->setColor(Qt::white); // 白色文字
            result.addResultShape(text);

            result.setMsg(QString("检测成功: %1 px").arg(totalLen));
        }
        else {
            result.setMsg("NG: 最终骨架为空");
        }

        result.setResultType(ResultType::OK);

    }
    catch (HalconCpp::HException& e) {
        result.setCode(-99);
        result.setMsg("Halcon 异常: " + QString(e.ErrorMessage().Text()));
        result.setResultType(ResultType::NG);
    }
    catch (std::exception& e) {
        result.setCode(-98);
        result.setMsg("C++ 异常: " + QString(e.what()));
        result.setResultType(ResultType::NG);
    }
    catch (...) {
        result.setCode(-97);
        result.setMsg("发生未知系统异常");
        result.setResultType(ResultType::NG);
    }

    return result;
}