#include "read_code_algorithm.h"
#include "roi_shape.h"
#include "result_shape.h"

ReadCodeAlgorithm::ReadCodeAlgorithm(): 
    prop_obj_(new ConfigurableObject()),
    init_param_status_(false)
{
    setupProperties();
}

ReadCodeAlgorithm::~ReadCodeAlgorithm()
{

}

bool ReadCodeAlgorithm::initialize()
{
    return true;
}

QString ReadCodeAlgorithm::name() const
{
    return "readcode";
}

void ReadCodeAlgorithm::setupProperties()
{
    PropertyBuilder::create("选择图像源", "image_source")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"图像1", ""} })
        .defaultValue("图像1")
        .registerTo(prop_obj_);

    PropertyBuilder::create("选择条码类型", "code_type")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"QR Code", 0},
                {"Data Matrix ECC 200", 1},
                {"PDF417", 2} })
        .defaultValue("QR Code")
        .registerTo(prop_obj_);

    PropertyBuilder::create("是否解析条码", "is_analysis")
        .category("基本参数")
        .type(QMetaType::Bool)
        .defaultValue(true)
        .registerTo(prop_obj_);

    prop_obj_->registerBtn(1, "测试运行");
}


void ReadCodeAlgorithm::setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p)
{
    for (auto it = p.begin(); it != p.end(); it++) {
        prop_obj_->setPropertyEnumValues(it.key(), it.value());
    }
}

ConfigurableObject* ReadCodeAlgorithm::getPropertyObj()
{
    return prop_obj_;
}

void ReadCodeAlgorithm::setParameters(const QJsonObject& obj)
{
    prop_obj_->setPropetyFromJson(obj);

    roishapes_.clear();
    for (auto roi : obj["rois"].toArray()) {
        roishapes_.append(RoiShape::fromJson(roi.toObject()));
    }

    init_param_status_ = true;
}

HTuple ReadCodeAlgorithm::corAngle(double angle)
{
    if (angle < 180)
        return HTuple(-angle);
    else
        return HTuple(360 - angle);
}

HObject ReadCodeAlgorithm::getRoiMaskImage(HObject& image, const QVector<std::shared_ptr<RoiShape>> shapes)
{
    HObject empRegion, empRegionMask;
    GenEmptyRegion(&empRegion);
    GenEmptyRegion(&empRegionMask);
    for (const auto roi : shapes) {
        switch (roi->type()) {
        case RoiShape::Polygon:{
            break;
        }
        case RoiShape::Circle: {
            auto shape = std::static_pointer_cast<RoiCircle>(roi);
            HObject Circle;
            GenCircle(&Circle, shape->center().y(), shape->center().x(), shape->radius());
            bool isMask = shape->isMasked();
            HalconCpp::ConcatObj(isMask ? empRegionMask : empRegion, Circle, isMask ? &empRegionMask : &empRegion);
            break;
        }            
        case RoiShape::Ring: {           
            auto shape = std::static_pointer_cast<RoiRing>(roi);
            HObject CircleOut, circleIn, ring;
            GenCircle(&CircleOut, shape->center().y(), shape->center().x(), shape->outterRadius());
            GenCircle(&circleIn, shape->center().y(), shape->center().x(), shape->innerRadius());
            HalconCpp::Difference(CircleOut, circleIn, &ring);
            bool isMask = shape->isMasked();
            HalconCpp::ConcatObj(isMask ? empRegionMask : empRegion, ring, isMask ? &empRegionMask : &empRegion);
            
            break;
        }
        case RoiShape::Rectangle: {
            auto shape = std::static_pointer_cast<RoiRect>(roi);
            HObject rectang;
            HTuple rows, cols;
            for (int i = 0; i < 4; i++) {
               rows.Append(HTuple(shape->vertex(i).y()));
               cols.Append(HTuple(shape->vertex(i).x()));
            }
            GenRegionPolygonFilled(&rectang, rows, cols);
            bool isMask = shape->isMasked();
            HalconCpp::ConcatObj(isMask ? empRegionMask : empRegion, rectang, isMask ? &empRegionMask : &empRegion);
            break;
        }
        case RoiShape::ArcRing: {
            auto shape = std::static_pointer_cast<RoiArcRing>(roi);
            if (shape) {
                HObject ContCircleOut, ContCircleIn;
                HObject Contour, RegionOut, RegionIn, RegionArc;
                HTuple Row, Col, Rows, Cols;
                HTuple cenRow = HTuple(shape->center().y());
                HTuple cenCol = HTuple(shape->center().x());
                HTuple radiusOut = HTuple(shape->outterRadius());
                HTuple radiusIn = HTuple(shape->innerRadius());
                HTuple anglestart = corAngle(shape->startAngle());
                HTuple angleEnd = corAngle(shape->startAngle() + shape->spanAngle());
                GenCircleContourXld(&ContCircleOut, cenRow, cenCol, radiusOut, angleEnd.TupleRad(), anglestart.TupleRad(), "positive", 1);
                GenCircleContourXld(&ContCircleIn, cenRow, cenCol, radiusIn, angleEnd.TupleRad(), anglestart.TupleRad(), "positive", 1);
                GetContourXld(ContCircleOut, &Row, &Col);
                Rows = HTuple();
                Cols = HTuple();
                Rows.Clear();
                Rows.Append(cenRow);
                Rows.Append(Row);
                Cols.Clear();
                Cols.Append(cenCol);
                Cols.Append(Col);
                Rows.Append(cenRow);
                Cols.Append(cenCol);
                GenContourPolygonXld(&Contour, Rows, Cols);
                GenRegionContourXld(Contour, &RegionOut, "filled");

                GetContourXld(ContCircleIn, &Row, &Col);
                Rows = HTuple();
                Cols = HTuple();
                Rows.Clear();
                Rows.Append(cenRow);
                Rows.Append(Row);
                Cols.Clear();
                Cols.Append(cenCol);
                Cols.Append(Col);
                Rows.Append(cenRow);
                Cols.Append(cenCol);
                GenContourPolygonXld(&Contour, Rows, Cols);
                GenRegionContourXld(Contour, &RegionIn, "filled");

                HalconCpp::Difference(RegionOut, RegionIn, &RegionArc);
                bool isMask = shape->isMasked();
                ConcatObj(isMask ? empRegionMask : empRegion, RegionArc, isMask ? &empRegionMask : &empRegion);
            }
            break;
        }
        default:
            break;
        }
    }
    HObject empRegionUnion, empRegionMaskUnion, finalRegion;
    HalconCpp::Union1(empRegion, &empRegionUnion);
    HalconCpp::Union1(empRegionMask, &empRegionMaskUnion);
    Difference(empRegionUnion, empRegionMaskUnion, &finalRegion);
    Intersection(image, finalRegion, &finalRegion);
    return finalRegion;
}

AlgorithmResult ReadCodeAlgorithm::test(const AlgorithmInput& input)
{
    AlgorithmResult result;

    HObject ho_SymbolXLDs;
    HObject imageReduced;
    HTuple hv_DataCodeHandle[2];
    HTuple hv_DataCodeHandleTrain[2];
    HTuple hv_ResultHandles1, hv_Ord;
    HTuple hv_DecodedDataStrings, hv_ResultValues, Length, Length2;
    HTuple hv_Row, hv_Col, hv_Index;
    HTuple modelLen1, modelLen2;

    int idx = prop_obj_->enumIndex("image_source");
    if (idx >= input.imageCount()) {
        result.setCode(-1);
        result.setMsg("图像索引超出范围");
        return result;
    }

    HObject hSrc = input.getImageAsHObject(idx);
    if (!hSrc.IsInitialized()) {
        result.setCode(-1);
        result.setMsg("获取图片失败");
        return result;
    }

    HObject ROI = getRoiMaskImage(hSrc, input.roiShapes());
    ReduceDomain(hSrc, ROI, &imageReduced);
    int index = 0;
    bool readSuccess = true;
    QString codeResult = "条码为空";
    QString codeType = prop_obj_->propValue("code_type").toString();
    bool isAnalysis = prop_obj_->propValue("is_analysis").toBool();

    if (codeType == "QR Code") {
        if (hv_DataCodeHandleTrain[index].Length() == 0) {
            CreateDataCode2dModel("QR Code", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
            SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
        }
        FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "未识别到QR二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
    }
    else if (codeType == "Data Matrix ECC 200") {
        if (hv_DataCodeHandle[index].Length() == 0) {
            CreateDataCode2dModel("Data Matrix ECC 200", "default_parameters", "enhanced_recognition", &hv_DataCodeHandle[index]);
            if (isAnalysis) {
                SetDataCode2dParam(hv_DataCodeHandle[index], "decoding_scheme", "raw");
            }
            SetDataCode2dParam(hv_DataCodeHandle[index], "symbol_size", 14);
            SetDataCode2dParam(hv_DataCodeHandle[index], "polarity", "any");
        }
        if (false) {
            FindDataCode2d(hSrc, &ho_SymbolXLDs, hv_DataCodeHandle[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        }
        else {
            FindDataCode2d(hSrc, &ho_SymbolXLDs, hv_DataCodeHandle[index], "train", "all", &hv_ResultHandles1, &hv_DecodedDataStrings);
        }
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "首次未识别到ECC二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
        else {
            if (hv_DataCodeHandleTrain[index].Length() == 0) {
                CreateDataCode2dModel("Data Matrix ECC 200", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
                if (prop_obj_->propValue("is_analysis").toBool()) {
                    SetDataCode2dParam(hv_DataCodeHandleTrain[index], "decoding_scheme", "raw");
                }
                SetDataCode2dParam(hv_DataCodeHandleTrain[index], "symbol_size", 14);
                SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
            }
            FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], "train", "all", &hv_ResultHandles1, &hv_DecodedDataStrings);
            TupleLength(hv_DecodedDataStrings, &Length2);
            if (Length2.I() != 1) {
                codeResult = "二次未识别到ECC二维码,识别长度：" + QString::number(Length2.I());
                HTuple modelLen1, modelLen2;
                TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
                if (modelLen1.I() >= 1) {
                    ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
                }
                TupleLength(hv_DataCodeHandle[index], &modelLen2);
                if (modelLen2.I() >= 1) {
                    ClearDataCode2dModel(hv_DataCodeHandle[index]);
                }
                readSuccess = false;
            }
        }
    }
    else if (codeType == "PDF417") {
        if (hv_DataCodeHandleTrain[index].Length() == 0) {
            CreateDataCode2dModel("PDF417", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
            SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
        }
        FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "未识别到PDF二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
    }

    if (readSuccess) {
        GetDataCode2dResults(hv_DataCodeHandleTrain[index], hv_ResultHandles1[0], "decoded_data", &hv_ResultValues);
        TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
        if (modelLen1.I() >= 1) {
            ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
        }
        TupleLength(hv_DataCodeHandle[index], &modelLen2);
        if (modelLen2.I() >= 1) {
            ClearDataCode2dModel(hv_DataCodeHandle[index]);
        }

        if (isAnalysis) {
            QString str;
            for (hv_Index = 0; hv_Index <= 7; hv_Index += 1) {
                try {
                    hv_Ord = HTuple(hv_ResultValues[hv_Index]);
                }
                catch (HException& HDevExpDefaultException) {
                    continue;
                }
                if (!hv_Ord.Length())
                    continue;
                int value = (hv_Ord[0].I() - 149 * (hv_Index[0].I() + 1) % 255 - 1) % 256;
                QString hex = QString("%1").arg(value, 2, 16, QLatin1Char('0'));
                QStringView hexTemp = QStringView(hex).right(2);
                str += hexTemp;
                codeResult = str.toUpper();
            }
        }
        else {
            codeResult = QString(hv_DecodedDataStrings[0].S().Text());
        }

        //! 四个角点坐标
        GetContourXld(ho_SymbolXLDs, &hv_Row, &hv_Col);
        QVector<QPointF> contour;
        for (int i = 0; i < hv_Row.Length() - 1; ++i) {
            // contour.push_back(QPointF(hv_Col[i].D(), hv_Row[i].D()));
            auto rect = std::make_shared<ResultRect>(QPointF(hv_Col[i].D(), hv_Row[i].D()), 4, 4, Qt::red);
            result.addResultShape(rect);
        }
    }

    result.addMetaData("codeResult", codeResult);
    return result;
}

AlgorithmResult ReadCodeAlgorithm::run(const AlgorithmContext& context)
{
    AlgorithmResult result;

    HObject ho_SymbolXLDs;
    HObject imageReduced;
    HTuple hv_DataCodeHandle[2];
    HTuple hv_DataCodeHandleTrain[2];
    HTuple hv_ResultHandles1, hv_Ord;
    HTuple hv_DecodedDataStrings, hv_ResultValues, Length, Length2;
    HTuple hv_Row, hv_Col, hv_Index;
    HTuple modelLen1, modelLen2;

    HObject hSrc = context.input.getImageAsHObject(0);
    if (!hSrc.IsInitialized()) {
        result.setCode(-1);
        result.setMsg("图像错误");
        return result;
    }

    if (!init_param_status_) {
        result.setCode(-1);
        result.setMsg("参数初始化失败");
        return result;
    }

    HObject ROI = getRoiMaskImage(hSrc, roishapes_);
    ReduceDomain(hSrc, ROI, &imageReduced);
    int index = 0;
    bool readSuccess = true;
    QString codeResult = "条码为空";
    QString codeType = prop_obj_->propValue("code_type").toString();
    bool isAnalysis = prop_obj_->propValue("is_analysis").toBool();

    if (codeType == "QR Code") {
        if (hv_DataCodeHandleTrain[index].Length() == 0) {
            CreateDataCode2dModel("QR Code", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
            SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
        }
        FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "未识别到QR二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
    }
    else if (codeType == "Data Matrix ECC 200") {
        if (hv_DataCodeHandle[index].Length() == 0) {
            CreateDataCode2dModel("Data Matrix ECC 200", "default_parameters", "enhanced_recognition", &hv_DataCodeHandle[index]);
            if (isAnalysis) {
                SetDataCode2dParam(hv_DataCodeHandle[index], "decoding_scheme", "raw");
            }
            SetDataCode2dParam(hv_DataCodeHandle[index], "symbol_size", 14);
            SetDataCode2dParam(hv_DataCodeHandle[index], "polarity", "any");
        }
        if (false) {
            FindDataCode2d(hSrc, &ho_SymbolXLDs, hv_DataCodeHandle[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        }
        else {
            FindDataCode2d(hSrc, &ho_SymbolXLDs, hv_DataCodeHandle[index], "train", "all", &hv_ResultHandles1, &hv_DecodedDataStrings);
        }
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "首次未识别到ECC二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
        else {
            if (hv_DataCodeHandleTrain[index].Length() == 0) {
                CreateDataCode2dModel("Data Matrix ECC 200", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
                if (prop_obj_->propValue("is_analysis").toBool()) {
                    SetDataCode2dParam(hv_DataCodeHandleTrain[index], "decoding_scheme", "raw");
                }
                SetDataCode2dParam(hv_DataCodeHandleTrain[index], "symbol_size", 14);
                SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
            }
            FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], "train", "all", &hv_ResultHandles1, &hv_DecodedDataStrings);
            TupleLength(hv_DecodedDataStrings, &Length2);
            if (Length2.I() != 1) {
                codeResult = "二次未识别到ECC二维码,识别长度：" + QString::number(Length2.I());
                HTuple modelLen1, modelLen2;
                TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
                if (modelLen1.I() >= 1) {
                    ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
                }
                TupleLength(hv_DataCodeHandle[index], &modelLen2);
                if (modelLen2.I() >= 1) {
                    ClearDataCode2dModel(hv_DataCodeHandle[index]);
                }
                readSuccess = false;
            }
        }
    }
    else if (codeType == "PDF417") {
        if (hv_DataCodeHandleTrain[index].Length() == 0) {
            CreateDataCode2dModel("PDF417", "default_parameters", "enhanced_recognition", &hv_DataCodeHandleTrain[index]);
            SetDataCode2dParam(hv_DataCodeHandleTrain[index], "polarity", "any");
        }
        FindDataCode2d(imageReduced, &ho_SymbolXLDs, hv_DataCodeHandleTrain[index], HTuple(), HTuple(), &hv_ResultHandles1, &hv_DecodedDataStrings);
        TupleLength(hv_DecodedDataStrings, &Length);
        if (Length.I() < 1) {
            codeResult = "未识别到PDF二维码,识别长度：" + QString::number(Length.I());
            HTuple modelLen1, modelLen2;
            TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
            if (modelLen1.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
            }
            TupleLength(hv_DataCodeHandle[index], &modelLen2);
            if (modelLen2.I() >= 1) {
                ClearDataCode2dModel(hv_DataCodeHandle[index]);
            }
            readSuccess = false;
        }
    }

    if (readSuccess) {
        GetDataCode2dResults(hv_DataCodeHandleTrain[index], hv_ResultHandles1[0], "decoded_data", &hv_ResultValues);
        TupleLength(hv_DataCodeHandleTrain[index], &modelLen1);
        if (modelLen1.I() >= 1) {
            ClearDataCode2dModel(hv_DataCodeHandleTrain[index]);
        }
        TupleLength(hv_DataCodeHandle[index], &modelLen2);
        if (modelLen2.I() >= 1) {
            ClearDataCode2dModel(hv_DataCodeHandle[index]);
        }

        if (isAnalysis) {
            QString str;
            for (hv_Index = 0; hv_Index <= 7; hv_Index += 1) {
                try {
                    hv_Ord = HTuple(hv_ResultValues[hv_Index]);
                }
                catch (HException& HDevExpDefaultException) {
                    continue;
                }
                if (!hv_Ord.Length())
                    continue;
                int value = (hv_Ord[0].I() - 149 * (hv_Index[0].I() + 1) % 255 - 1) % 256;
                QString hex = QString("%1").arg(value, 2, 16, QLatin1Char('0'));
                QStringView hexTemp = QStringView(hex).right(2);
                str += hexTemp;
                codeResult = str.toUpper();
            }
        }
        else {
            codeResult = QString(hv_DecodedDataStrings[0].S().Text());
        }

        //! 四个角点坐标
        GetContourXld(ho_SymbolXLDs, &hv_Row, &hv_Col);
        QVector<QPointF> contour;
        for (int i = 0; i < hv_Row.Length() - 1; ++i) {
            // contour.push_back(QPointF(hv_Col[i].D(), hv_Row[i].D()));
            auto rect = std::make_shared<ResultRect>(QPointF(hv_Col[i].D(), hv_Row[i].D()), 4, 4, Qt::red);
            result.addResultShape(rect);
        }

        QString resultText = "识别结果：" + codeResult;
        auto text = std::make_shared<ResultText>(resultText, QPointF(0, 0), 32, Qt::blue);
        result.addResultShape(text);

        QVariantList list;
        for (const auto& p : contour) list << QVariant::fromValue(p);
        result.addMetaData("contour", list);
    }

    result.addMetaData("codeResult", codeResult);
    result.setResultType(readSuccess ? ResultType::OK : ResultType::NG);
    return result;
}