#include "read_code_view.h"
#include <QVBoxLayout>
#include <QDateTime>
#include "algo_page_manager.h"
#include "algorithm_manager.h"
#include "read_code_algorithm.h"

ReadCodeView::ReadCodeView(AlgoPageManager* pageMgr, QString type, QWidget* parent) :
    PageBase(pageMgr, type, parent), page_id_("")
{
    read_code_ = new ReadCodeAlgorithm();
    if (!read_code_) {
        qDebug() << "read code algorithm load error";
        return;
    }
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    property_editor_ = new PropertyEditorWidget(read_code_->getPropertyObj());
    algo_content_view_ = new AlgoContentView(property_editor_, this);
    mainLayout->addWidget(algo_content_view_);
    algo_content_view_->imageEditor()->setHeaderVisible(true);

    setImageSourceProp();
    setImageSource();
    connect(algo_content_view_->propEditor(), &PropertyEditorWidget::onBtnCliclkSignals, this, &ReadCodeView::onPropBtnClickedSlots);
    connect(manager(), &AlgoPageManager::pageAdded, this, &ReadCodeView::onPageChangedSlots);
    connect(manager(), &AlgoPageManager::pageRemoved, this, &ReadCodeView::onPageChangedSlots);
    connect(algo_content_view_->propEditor()->propObject(), &ConfigurableObject::propertyChanged, this, &ReadCodeView::onPropValueChanged);
}

ReadCodeView::~ReadCodeView()
{
    delete read_code_;
    algo_content_view_->deleteLater();
}

void ReadCodeView::clearResults()
{
    algo_content_view_->imageEditor()->clearAllResult();
}

void ReadCodeView::setImageSourceProp()
{
    page_ids_ = manager()->getPageIdsByName("imagegrab");
    setImageSourcePropEnums(page_ids_);
    algo_content_view_->propEditor()->updateEditorValue("image_source");
}

void ReadCodeView::setImageSource()
{
    QString pageId = algo_content_view_->propEditor()->propObject()->enumData("image_source").toString();
    if (pageId != page_id_) {
        page_id_ = pageId;
        algo_content_view_->imageEditor()->clearRois();
        clearResults();
    }
    QImage img = manager()->getPageImageById(page_id_);
    algo_content_view_->imageEditor()->setImage(img);
}

void ReadCodeView::onPageChangedSlots(QString pageName)
{
    if (pageName == "imagegrab") {
        setImageSourceProp();
    }
}

void ReadCodeView::onPropValueChanged(QString key, QVariant value)
{
    if (key == "image_source") {
        setImageSource();
    }
}

void ReadCodeView::onPropBtnClickedSlots(QString btnName)
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

        QVector<std::shared_ptr<ROIItem>> rois = algo_content_view_->imageEditor()->getAllROIs();
        if (rois.size() <= 0) {
            qDebug() << "请先绘制ROI区域";
            return;
        }

        input.setRoiShapes(algo_content_view_->imageEditor()->getRoiShapes());
        AlgorithmResult result = read_code_->test(input);

        algo_content_view_->imageEditor()->clearAllResult();

        if (result.isSuccess()) {
            QVariantMap meta = result.metaData();
            meta = meta;
            algo_content_view_->info("识别成功： 识别结果为->" + meta["codeResult"].toString());
            algo_content_view_->imageEditor()->setResultShapes(result.resultShapes());
        }
        else {
            qDebug() << "扫码失败:" << result.metaData()["error"].toString();
        }
    }
}

void ReadCodeView::onPageActivated(const QVariant& data)
{
    setImageSource();
}

void ReadCodeView::setImageSourcePropEnums(QStringList list)
{
    QList<QPair<QString, QVariant>> enums;
    for (int i = 0; i < list.size(); i++) {
        enums.append(qMakePair(QString("图像%1").arg(i + 1), list[i]));
    }
    QMap<QString, QList<QPair<QString, QVariant>>> runParams;
    runParams["image_source"] = enums;
    read_code_->setRunParameters(runParams);
}

QJsonObject ReadCodeView::algoParameters() const
{
    QJsonObject obj = property_editor_->getPropJson();
    obj["rois"] = algo_content_view_->imageEditor()->getRoiParameters();
    obj["image"] = modelImgName();
    return obj;
}

void ReadCodeView::setAlgoParameters(const QJsonObject& obj)
{
    if (!obj["image"].isNull()) {
        QImage img(obj["image"].toString());
        if (!img.isNull()) {
            algo_content_view_->imageEditor()->setImage(img);
        }
    }
    read_code_->setParameters(obj);
    algo_content_view_->imageEditor()->loadRoi(obj);
}

void ReadCodeView::saveImage()
{
    QImage image = algo_content_view_->imageEditor()->getImage();
    if (!image.isNull()) {
        image.save(modelImgName());
    }
}