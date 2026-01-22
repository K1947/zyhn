#include "circle_fitter_view.h"
#include <QVBoxLayout>
#include <QDateTime>
#include "algo_page_manager.h"
#include "algorithm_manager.h"
#include "circlefit_algorithm.h"


CircleFitterView::CircleFitterView(AlgoPageManager* pageMgr, QString type, QWidget* parent) :
    PageBase(pageMgr, type, parent), page_id_(""), save_model_img_(false)
{
    circle_fitter_ = new CircleFitAlgorithm();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    property_editor_ = new PropertyEditorWidget(circle_fitter_->getPropertyObj());
    algo_content_view_ = new AlgoContentView(property_editor_, this);
    mainLayout->addWidget(algo_content_view_);
    algo_content_view_->imageEditor()->setHeaderVisible(true);

    setImageSourceProp();
    setImageSource();
    connect(algo_content_view_->propEditor(), &PropertyEditorWidget::onBtnCliclkSignals, this, &CircleFitterView::onPropBtnClickedSlots);
    connect(manager(), &AlgoPageManager::pageAdded, this, &CircleFitterView::onPageChangedSlots);
    connect(manager(), &AlgoPageManager::pageRemoved, this, &CircleFitterView::onPageChangedSlots);
    connect(algo_content_view_->propEditor()->propObject(), &ConfigurableObject::propertyChanged, this, &CircleFitterView::onPropValueChanged);
}

CircleFitterView::~CircleFitterView()
{
    delete circle_fitter_;
    algo_content_view_->deleteLater();
}

void CircleFitterView::clearResults()
{
    // 显示界面清除所有结果
    algo_content_view_->imageEditor()->clearAllResult();
}

void CircleFitterView::setImageSourceProp()
{
    page_ids_ = manager()->getPageIdsByName("imagegrab");
    setImageSourcePropEnums(page_ids_);
    algo_content_view_->propEditor()->updateEditorValue("image_source");
}

void CircleFitterView::  setImageSource()
{
    if (save_model_img_) {
        return;
    }

    QString pageId = algo_content_view_->propEditor()->propObject()->enumData("image_source").toString();
    if (pageId != page_id_) {
        page_id_ = pageId;
        algo_content_view_->imageEditor()->clearRois();
        clearResults();
    }
    QImage img = manager()->getPageImageById(page_id_);
    algo_content_view_->imageEditor()->setImage(img);
}

void CircleFitterView::onPageChangedSlots(QString pageName)
{
    if (pageName == "imagegrab") {
        setImageSourceProp();
    }
}

void CircleFitterView::onPropValueChanged(QString key, QVariant value)
{
    if (key == "image_source") {
        setImageSource();
    }
    else {
        if (algo_content_view_->propEditor()->propObject()->isRealUpdate()) {
            onPropBtnClickedSlots("测试运行");
        }
    }
}

void CircleFitterView::onPropBtnClickedSlots(QString btnName)
{
    if (btnName == "测试运行") {
        AlgorithmInput input;
        QStringList pageIds = manager()->getPageIdsByName("imagegrab");
        for (auto pageId : pageIds) {
            QImage img = manager()->getPageImageById(pageId);
            if (img.isNull()) {
                continue;
            }
            input.addImage(img);
        }
        //  判断采集图像数量为空则从界面上读取图像
        if (input.imageCount() <= 0) {
            const QImage& image = algo_content_view_->imageEditor()->getImage();
            if (!image.isNull()) {
                input.addImage(image);
            }
        }

        // 判断界面绘制的ROI是否正确
        auto rois = algo_content_view_->imageEditor()->getROIsByType(ROIType::Ring);
        if (rois.size() <= 0) {
            qDebug() << "请先绘制环形ROI区域";
            return;
        }
        // 获取界面已绘制的所有ROI
        input.setRoiShapes(algo_content_view_->imageEditor()->getRoiShapes());  
        // 调用圆拟合测试函数
        AlgorithmResult result = circle_fitter_->test(input);
        // 清空编辑界面结果
        algo_content_view_->imageEditor()->clearAllResult();
        // 根据算法返回结果实时输出
        if (result.isSuccess()) {
            QVariantMap meta = result.metaData();          
            algo_content_view_->imageEditor()->setResultShapes(result.resultShapes());
        } else {
            qDebug() << "圆拟合失败:" << result.metaData()["error"].toString();
        }
    }
    else if (btnName == "保存模板图像") {
        saveImage();
        save_model_img_ = true;
        algo_content_view_->debug("保存模板图像成功");
    }
}

void CircleFitterView::onPageActivated(const QVariant& data)
{
    setImageSource();
}

void CircleFitterView::setImageSourcePropEnums(QStringList list)
{
    QList<QPair<QString, QVariant>> enums;
    for (int i = 0; i < list.size(); i++) {
        enums.append(qMakePair(QString("图像%1").arg(i + 1), list[i]));
    }
    QMap<QString, QList<QPair<QString, QVariant>>> runParams;
    runParams["image_source"] = enums;
    circle_fitter_->setRunParameters(runParams);
}

QJsonObject CircleFitterView::algoParameters() const
{
    QJsonObject obj = property_editor_->getPropJson();
    obj["rois"] = algo_content_view_->imageEditor()->getRoiParameters();
    if (save_model_img_) {
        obj["model_image"] = modelImgName();
    }
    return obj;
}

void CircleFitterView::setAlgoParameters(const QJsonObject& obj)
{
    if (!obj["model_image"].isNull()) {
        QImage img(obj["model_image"].toString());
        if (!img.isNull()) {
            save_model_img_ = true;
            algo_content_view_->imageEditor()->setImage(img);
        }
    }
    algo_content_view_->propEditor()->setPropValue(obj);
    algo_content_view_->imageEditor()->loadRoi(obj);
}

void CircleFitterView::saveImage()
{
    QImage image = algo_content_view_->imageEditor()->getImage();
    if (!image.isNull()) {
        image.save(modelImgName());
    }
}