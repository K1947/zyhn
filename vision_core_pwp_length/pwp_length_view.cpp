#include "pwp_length_view.h"
#include <QVBoxLayout>
#include "algo_page_manager.h"

PwpLengthView::PwpLengthView(AlgoPageManager* pageMgr, QString type, QWidget* parent) :
    PageBase(pageMgr, type, parent), page_id_("")
{
    algo_instance_ = new PwpLengthAlgorithm();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    property_editor_ = new PropertyEditorWidget(algo_instance_->getPropertyObj());
    algo_content_view_ = new AlgoContentView(property_editor_, this);
    mainLayout->addWidget(algo_content_view_);
    algo_content_view_->imageEditor()->setHeaderVisible(true);

    setImageSourceProp();
    setImageSource();

    // 绑定信号
    connect(algo_content_view_->propEditor(), &PropertyEditorWidget::onBtnCliclkSignals, this, &PwpLengthView::onPropBtnClickedSlots);
    connect(manager(), &AlgoPageManager::pageAdded, this, &PwpLengthView::onPageChangedSlots);
    connect(manager(), &AlgoPageManager::pageRemoved, this, &PwpLengthView::onPageChangedSlots);
    connect(algo_content_view_->propEditor()->propObject(), &ConfigurableObject::propertyChanged, this, &PwpLengthView::onPropValueChanged);
}

PwpLengthView::~PwpLengthView()
{
    delete algo_instance_;
    algo_content_view_->deleteLater();
}

void PwpLengthView::clearResults()
{
    algo_content_view_->imageEditor()->clearAllResult();
}

void PwpLengthView::setImageSourceProp()
{
    page_ids_ = manager()->getPageIdsByName("imagegrab");
    setImageSourcePropEnums(page_ids_);
    algo_content_view_->propEditor()->updateEditorValue("image_source");
}

void PwpLengthView::setImageSource()
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

void PwpLengthView::onPageChangedSlots(QString pageName)
{
    if (pageName == "imagegrab") {
        setImageSourceProp();
    }
}

void PwpLengthView::onPropValueChanged(QString key, QVariant value)
{
    if (key == "image_source") {
        setImageSource();
    }
}

void PwpLengthView::onPropBtnClickedSlots(QString btnName)
{
    if (btnName == "测试运行") {
        AlgorithmInput input;

        // 1. 准备图像
        QStringList pageIds = manager()->getPageIdsByName("imagegrab");
        for (auto pageId : pageIds) {
            QImage img = manager()->getPageImageById(pageId);
            if (!img.isNull()) input.addImage(img);
        }
        if (input.imageCount() <= 0) {
            const QImage& image = algo_content_view_->imageEditor()->getImage();
            if (!image.isNull()) input.addImage(image);
        }

        // 2. 准备 ROI
        input.setRoiShapes(algo_content_view_->imageEditor()->getRoiShapes());

        // 3. 运行算法
        AlgorithmResult result = algo_instance_->test(input);

        // 4. 显示结果
        algo_content_view_->imageEditor()->clearAllResult();
        if (result.isSuccess()) {
            algo_content_view_->imageEditor()->setResultShapes(result.resultShapes());
            algo_content_view_->info(result.msg());
        }
        else {
            algo_content_view_->error("算法失败: " + result.msg());
        }
    }
}

void PwpLengthView::onPageActivated(const QVariant& data)
{
    setImageSource();
}

void PwpLengthView::setImageSourcePropEnums(QStringList list)
{
    QList<QPair<QString, QVariant>> enums;
    for (int i = 0; i < list.size(); i++) {
        enums.append(qMakePair(QString("图像%1").arg(i + 1), list[i]));
    }
    QMap<QString, QList<QPair<QString, QVariant>>> runParams;
    runParams["image_source"] = enums;
    algo_instance_->setRunParameters(runParams);
}

QJsonObject PwpLengthView::algoParameters() const
{
    QJsonObject obj = property_editor_->getPropJson();
    obj["rois"] = algo_content_view_->imageEditor()->getRoiParameters();
    return obj;
}

void PwpLengthView::setAlgoParameters(const QJsonObject& obj)
{
    algo_content_view_->propEditor()->setPropValue(obj);
    algo_content_view_->imageEditor()->loadRoi(obj);
}

void PwpLengthView::saveImage()
{
    QImage image = algo_content_view_->imageEditor()->getImage();
    if (!image.isNull()) {
        image.save(modelImgName());
    }
}