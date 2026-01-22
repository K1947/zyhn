#include "hairy_fabric_algorithm.h"
#include "property_editor_widget.h"
#include "result_shape.h"
#include "logger_utils.h"
#include "roi_shape.h"

#include <QPainterPath>
#include <memory>
#include <exception>
#include <cmath>
#include <QDebug>



using namespace HalconCpp;

//用于格式转换
HObject QImageRGB32ToHObject(const QImage& qImg)
{
    HObject ho_Image;
    if (qImg.isNull() || qImg.format() != QImage::Format_RGB32) {
        std::cerr << "错误：输入QImage为空或格式不是RGB32" << std::endl;
        return ho_Image;
    }
    int width = qImg.width();
    int height = qImg.height();
    // RGB32 stride 必须是 width * 4，但在某些对齐下可能不同，严谨的做法是用 bytesPerLine
    // 但 Halcon GenImageInterleaved 默认行连续。这里假设是紧凑排列。
    int bufferSize = width * height * 4;

    uchar* pBuffer = new(std::nothrow) uchar[bufferSize];
    if (pBuffer == nullptr) return ho_Image;
    std::memcpy(pBuffer, qImg.bits(), bufferSize);

    try {
        GenImageInterleaved(&ho_Image, (Hlong)pBuffer, "bgrx",
            width, height, 0, "byte", width, height, 0, 0, -1, 0);
    }
    catch (const HOperatorException& e) { ho_Image = HObject(); }

    delete[] pBuffer;
    return ho_Image;
}

//正式
HairyFabricAlgorithm::HairyFabricAlgorithm() :
    prop_obj_(new ConfigurableObject()),
    init_param_status_(true)
{
    setupProperties();
}

HairyFabricAlgorithm::~HairyFabricAlgorithm()
{
    if (prop_obj_) {
        delete prop_obj_;
        prop_obj_ = nullptr;
    }
}

bool HairyFabricAlgorithm::initialize() { return true; }
QString HairyFabricAlgorithm::name() const { return "hairy_fabric"; }

void HairyFabricAlgorithm::setupProperties()
{
    PropertyBuilder::create("选择图像源", "image_source")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"图像1", ""} })
        .defaultValue("图像1")
        .registerTo(prop_obj_);

    // --- 预处理参数 ---
    PropertyBuilder::create("蓝色通道索引", "blue_channel")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(3) 
        .registerTo(prop_obj_);

    PropertyBuilder::create("平滑宽/高(Mean)", "mean_mask_size")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(25)
        .registerTo(prop_obj_);

    PropertyBuilder::create("二值化下限", "threshold_min")
        .category("预处理")
        .type(QMetaType::Int)
        .defaultValue(165)
        .registerTo(prop_obj_);

    PropertyBuilder::create("最大区域相似度", "std_percent")
        .category("预处理")
        .type(QMetaType::Double)
        .defaultValue(70.0)
        .registerTo(prop_obj_);

    PropertyBuilder::create("大开运算半径", "opening_radius_large")
        .category("预处理")
        .type(QMetaType::Double)
        .defaultValue(12.5)
        .registerTo(prop_obj_);

    // --- 模式判定参数 ---
    PropertyBuilder::create("矩形长度阈值", "rect_len_th")
        .category("模式判定")
        .type(QMetaType::Double)
        .defaultValue(350.0) 
        .registerTo(prop_obj_);

    PropertyBuilder::create("矩形填充率阈值", "rect_ratio_th")
        .category("模式判定")
        .type(QMetaType::Double)
        .defaultValue(0.72) 
        .registerTo(prop_obj_);

    // --- 骨架提取参数 ---
    PropertyBuilder::create("骨架腐蚀半径", "skel_erosion")
        .category("骨架参数")
        .type(QMetaType::Double)
        .defaultValue(2.5) 
        .registerTo(prop_obj_);

    PropertyBuilder::create("骨架平滑系数", "smooth_sigma")
        .category("骨架参数")
        .type(QMetaType::Int)
        .defaultValue(41) 
        .registerTo(prop_obj_);

    prop_obj_->registerBtn(1, "测试运行");
    prop_obj_->registerBtn(2, "保存模板图像");
}

ConfigurableObject* HairyFabricAlgorithm::getPropertyObj() { return prop_obj_; }
void HairyFabricAlgorithm::setParameters(const QJsonObject& p) { prop_obj_->setPropetyFromJson(p); init_param_status_ = true; }
void HairyFabricAlgorithm::setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p) {
    for (auto it = p.begin(); it != p.end(); it++) prop_obj_->setPropertyEnumValues(it.key(), it.value());
}
AlgorithmResult HairyFabricAlgorithm::run(const AlgorithmContext& context) { return test(context.input); }

// 辅助：调试显示
void addDebugRegion(AlgorithmResult& result, const HObject& region, const QColor& color) {
    if (!region.IsInitialized()) return;
    HTuple count; CountObj(region, &count);
    if (count.I() == 0) return;
    int limit = (count.I() > 20) ? 20 : count.I();
    for (int i = 1; i <= limit; ++i) {
        HObject single; SelectObj(region, &single, i);
        HTuple rows, cols;
        try { GetRegionPolygon(single, 5.0, &rows, &cols); } catch(...) { continue; }
        if (rows.Length() > 0) {
            QPainterPath path;
            path.moveTo(cols[0].D(), rows[0].D());
            for (int k = 1; k < rows.Length(); ++k) path.lineTo(cols[k].D(), rows[k].D());
            path.closeSubpath();
            result.addResultShape(std::make_shared<ResultPath>(path, color));
        }
    }
}

AlgorithmResult HairyFabricAlgorithm::test(const AlgorithmInput& input)
{
    AlgorithmResult result;
    if (!init_param_status_) { result.setCode(-3); result.setMsg("参数未初始化"); return result; }

    try {
        // 获取图像
        if (input.imageCount() <= 0) { result.setCode(-1); result.setMsg("无图像"); return result; }
        
        // 这里的 getImageAsHObject 会根据 QImage 形式转换，封装起来看不到其该函数内容，但转换出来的均为单通道HObject形式，无法在后面提取blue通道
        QImage qImg = input.getQImage(0);
        qDebug() << "[HairyFabric] Detected Channels:" << qImg.format();
        HObject hImg = QImageRGB32ToHObject(qImg);
        if (!hImg.IsInitialized())
                 {
                     qDebug() << "图像转换失败";
                     return result;
                 }
        // 获取参数
        int blueCh = prop_obj_->propValue("blue_channel").toInt();
        int meanMask = prop_obj_->propValue("mean_mask_size").toInt();
        int thMin = prop_obj_->propValue("threshold_min").toInt();
        double stdPercent = prop_obj_->propValue("std_percent").toDouble();
        double openRadLarge = prop_obj_->propValue("opening_radius_large").toDouble();
        
        double rectLenTh = prop_obj_->propValue("rect_len_th").toDouble();
        double rectRatioTh = prop_obj_->propValue("rect_ratio_th").toDouble();
        double skelEro = prop_obj_->propValue("skel_erosion").toDouble();
        int smoothSig = prop_obj_->propValue("smooth_sigma").toInt();

        // ROI 填充 
        HObject ho_ROI_Search; GenEmptyObj(&ho_ROI_Search);
        bool has_valid_roi = false;
        
        if (input.roiShapes().size() > 0) {
            for (const auto& shape : input.roiShapes()) {
                if (!shape) continue;
                HObject ho_Temp; bool conv = false;
                
                try {
                    //多边形
                    if (shape->type() == RoiShape::Polygon) {
                        auto poly = std::dynamic_pointer_cast<RoiPolygon>(shape);
                        if (poly && poly->getPts().size() >= 3) {
                            HTuple r, c;
                            for(auto& p : poly->getPts()) { r.Append(p.y()); c.Append(p.x()); }
                            HObject cont; GenContourPolygonXld(&cont, r, c);
                            GenRegionContourXld(cont, &ho_Temp, "filled");
                            conv = true;
                        }
                    } 
                    //长方形
                    else if (shape->type() == RoiShape::Rectangle) {
                        QRectF r = shape->boundingRect();
                        GenRectangle1(&ho_Temp, r.top(), r.left(), r.bottom(), r.right());
                        conv = true;
                    } 
                    //圆形
                    else if (shape->type() == RoiShape::Circle) {
                        QRectF r = shape->boundingRect();
                        GenCircle(&ho_Temp, r.center().y(), r.center().x(), r.width()/2.0);
                        conv = true;
                    }

                    if (conv) {
                        if (!has_valid_roi) { ho_ROI_Search = ho_Temp; has_valid_roi = true; }
                        else { Union2(ho_ROI_Search, ho_Temp, &ho_ROI_Search); }
                    }
                } catch (...) {
                    qDebug() << "ROI 转换异常，跳过该形状";
                }
            }
        }

        HTuple w, h; GetImageSize(hImg, &w, &h);
        HObject ho_ImageRect; GenRectangle1(&ho_ImageRect, 0, 0, h - 1, w - 1);

        if (!has_valid_roi) {
            ho_ROI_Search = ho_ImageRect;
        } else {
            Intersection(ho_ROI_Search, ho_ImageRect, &ho_ROI_Search);
            addDebugRegion(result, ho_ROI_Search, Qt::blue);
        }

        HTuple roiArea; AreaCenter(ho_ROI_Search, &roiArea, 0, 0);
        if (roiArea.D() <= 0) {
            result.setMsg("ROI 无效 (Area=0)");
            result.setResultType(ResultType::NG);
            return result;
        }
        
        // 4. 核心算法流程 (Halcon 代码移植)
        HObject ho_ChannelImg, ho_Reduced, ho_Scaled, ho_Mean, ho_MeanScaled, ho_White;
        HObject ho_Connected, ho_Main, ho_Filled, ho_Smooth;
        HTuple hv_L1, hv_L2, hv_Phi, hv_Row, hv_Col, hv_Area, hv_AreaObj, hv_R1, hv_C1;

        // ... 通道获取 ...
        HTuple channels;
        CountChannels(hImg, &channels);
        if (channels.I() >= 3) {
            // 如果上面转换用了 "bgrx"，这里 3 就是真正的 Blue 通道
            AccessChannel(hImg, &ho_ChannelImg, blueCh);
        }
        else {
            AccessChannel(hImg, &ho_ChannelImg, 1);
        }

        ReduceDomain(ho_ChannelImg, ho_ROI_Search, &ho_Reduced);

        // 步骤 1: 第一次拉伸
        ScaleImageMax(ho_Reduced, &ho_Scaled);

        // 步骤 2: 均值滤波
        MeanImage(ho_Scaled, &ho_Mean, meanMask, meanMask);

        // 步骤 3: 【新增】第二次拉伸 (Halcon 脚本中有这一步)
        ScaleImageMax(ho_Mean, &ho_MeanScaled);

        // 步骤 4: 二值化 (对第二次拉伸后的图进行)
        Threshold(ho_MeanScaled, &ho_White, thMin, 255);

        addDebugRegion(result, ho_White, Qt::red);


        Connection(ho_White, &ho_Connected);
        SelectShapeStd(ho_Connected, &ho_Main, "max_area", stdPercent);
        
        HTuple countMain; CountObj(ho_Main, &countMain);
        if (countMain.I() == 0) {
            result.setMsg("NG: 未找到主要区域 (Threshold/SelectShapeStd 失败)");
            result.setResultType(ResultType::OK); 
            // 即使失败，也尽量把中间结果画出来
            addDebugRegion(result, ho_White, Qt::red);
            return result;
        }

        FillUp(ho_Main, &ho_Filled);
        OpeningCircle(ho_Filled, &ho_Smooth, openRadLarge);
        
        addDebugRegion(result, ho_Smooth, QColor(0, 255, 0, 80));

        SmallestRectangle2(ho_Smooth, &hv_Row, &hv_Col, &hv_Phi, &hv_L1, &hv_L2);
        AreaCenter(ho_Smooth, &hv_AreaObj, &hv_R1, &hv_C1);
        
        double areaRect = hv_L1.D() * hv_L2.D() * 4.0;
        double ratio = (areaRect > 0) ? (hv_AreaObj.D() / areaRect) : 0.0;
        double roughLen = hv_L1.D() * 2.0;

        double finalLength = 0.0;
        QString modeStr = "";

        if (roughLen <= rectLenTh && ratio > rectRatioTh) {
            // === 模式 A: 矩形模式 ===
            modeStr = "Rectangle";
            finalLength = roughLen;

            HObject ho_RectContour;
            GenRectangle2ContourXld(&ho_RectContour, hv_Row, hv_Col, hv_Phi, hv_L1, hv_L2);
            HTuple r, c; GetContourXld(ho_RectContour, &r, &c);
            
            QPainterPath rectPath;
            if(r.Length() > 0) {
                rectPath.moveTo(c[0].D(), r[0].D());
                for(int k=1; k<r.Length(); ++k) rectPath.lineTo(c[k].D(), r[k].D());
                rectPath.closeSubpath();
            }
            result.addResultShape(std::make_shared<ResultPath>(rectPath, Qt::green));

            double dRow = -std::sin(hv_Phi.D()) * hv_L1.D();
            double dCol = std::cos(hv_Phi.D()) * hv_L1.D();
            
            QPainterPath linePath;
            linePath.moveTo(hv_Col.D() - dCol, hv_Row.D() - dRow);
            linePath.lineTo(hv_Col.D() + dCol, hv_Row.D() + dRow);
            result.addResultShape(std::make_shared<ResultPath>(linePath, Qt::red));

        } else {
            // === 模式 B: 骨架提取模式 ===
            modeStr = "Skeleton";
            
            HObject ho_Core, ho_Skel, ho_SkelCont, ho_Split, ho_Cand;
            HObject ho_UnionCol, ho_FinalUnion, ho_BestLine, ho_Smoothed;

            ErosionCircle(ho_Smooth, &ho_Core, skelEro);
            Skeleton(ho_Core, &ho_Skel);
            GenContoursSkeletonXld(ho_Skel, &ho_SkelCont, 1, "filter");
            SegmentContoursXld(ho_SkelCont, &ho_Split, "lines_circles", 1, 1, 1);
            SelectContoursXld(ho_Split, &ho_Cand, "contour_length", 10, 9999999, -0.5, 0.5);
            
            UnionCollinearContoursXld(ho_Cand, &ho_UnionCol, 100, 2.0, 20, 0.7, "attr_keep");
            UnionAdjacentContoursXld(ho_UnionCol, &ho_FinalUnion, 20, 1, "attr_keep");
            SelectContoursXld(ho_FinalUnion, &ho_BestLine, "contour_length", 100, 9999999, -0.5, 0.5);

            HTuple numCand; CountObj(ho_BestLine, &numCand);
            if (numCand.I() > 0) {
                HObject ho_FinalSkel;
                if (numCand.I() > 1) {
                    HTuple lens, indices;
                    LengthXld(ho_BestLine, &lens);
                    TupleSortIndex(lens, &indices);
                    SelectObj(ho_BestLine, &ho_FinalSkel, indices[numCand.I()-1].I() + 1);
                } else {
                    CopyObj(ho_BestLine, &ho_FinalSkel, 1, 1);
                }

                SmoothContoursXld(ho_FinalSkel, &ho_Smoothed, smoothSig);
                HTuple len; LengthXld(ho_Smoothed, &len);
                finalLength = len.D();

                HTuple r, c; GetContourXld(ho_Smoothed, &r, &c);
                QPainterPath skelPath;
                if (r.Length() > 0) {
                    skelPath.moveTo(c[0].D(), r[0].D());
                    for(int k=1; k<r.Length(); ++k) skelPath.lineTo(c[k].D(), r[k].D());
                    result.addResultShape(std::make_shared<ResultPath>(skelPath, Qt::red));
                }
            } else {
                result.setMsg("NG: 骨架提取失败 (目标太碎)");
                result.setResultType(ResultType::OK);
                return result;
            }
        }

        auto text = std::make_shared<ResultText>();
        text->text_ = QString("L: %1 (%2)").arg(finalLength, 0, 'f', 1).arg(modeStr);
        text->text_pos_ = QPointF(20, 50);
        text->setColor(Qt::yellow);
        text->font_size_ = 20;
        result.addResultShape(text);

        result.setMsg(QString("Success! Length: %1").arg(finalLength));
        result.setResultType(ResultType::OK);

    } catch (HalconCpp::HException& e) {
        result.setCode(-99);
        result.setMsg("Halcon 异常: " + QString(e.ErrorMessage().Text()));
        result.setResultType(ResultType::NG);
    } catch (...) {
        result.setCode(-97);
        result.setMsg("未知异常");
        result.setResultType(ResultType::NG);
    }
    return result;
}