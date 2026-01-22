#include "hairy_fabric_view.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include "algo_page_manager.h"
#include "algorithm_manager.h"

HairyFabricView::HairyFabricView(AlgoPageManager* pageMgr, QString type, QWidget* parent) :
    PageBase(pageMgr, type, parent), page_id_(""), save_model_img_(false)
{
    algo_instance_ = new HairyFabricAlgorithm();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    property_editor_ = new PropertyEditorWidget(algo_instance_->getPropertyObj());
    algo_content_view_ = new AlgoContentView(property_editor_, this);
    mainLayout->addWidget(algo_content_view_);
    algo_content_view_->imageEditor()->setHeaderVisible(true);

    setImageSourceProp();
    setImageSource();

    connect(algo_content_view_->propEditor(), &PropertyEditorWidget::onBtnCliclkSignals, this, &HairyFabricView::onPropBtnClickedSlots);
    connect(manager(), &AlgoPageManager::pageAdded, this, &HairyFabricView::onPageChangedSlots);
    connect(manager(), &AlgoPageManager::pageRemoved, this, &HairyFabricView::onPageChangedSlots);
    connect(algo_content_view_->propEditor()->propObject(), &ConfigurableObject::propertyChanged, this, &HairyFabricView::onPropValueChanged);
}

HairyFabricView::~HairyFabricView()
{
    if (algo_instance_) delete algo_instance_;
}

// === 核心修改：在此处强制转换图像格式 ===
void HairyFabricView::onPropBtnClickedSlots(QString btnName)
{
    if (btnName == "测试运行") {
        AlgorithmInput input;

        // 1. 尝试从采集页获取图像
        QStringList pageIds = manager()->getPageIdsByName("imagegrab");
        bool hasImage = false;

        for (auto pageId : pageIds) {
            QImage img = manager()->getPageImageById(pageId);
            if (!img.isNull()) {
                //看格式
                qDebug() << "[HairyFabric] Detected Channels:" << img.format();
                input.addImage(img);
                hasImage = true;
            }
        }

        // 2. 如果没有采集图，使用当前编辑器中的图（例如加载的模板）
        if (!hasImage && input.imageCount() <= 0) {
            QImage image = algo_content_view_->imageEditor()->getImage();
            if (!image.isNull()) {
                input.addImage(image);
                qDebug() << "[HairyFabric] Detected Channels:" << image.format();
            }
        }

        // 传递 ROI
        input.setRoiShapes(algo_content_view_->imageEditor()->getRoiShapes());

        AlgorithmResult result = algo_instance_->test(input);

        algo_content_view_->imageEditor()->clearAllResult();
        if (result.isSuccess()) {
            algo_content_view_->imageEditor()->setResultShapes(result.resultShapes());
            algo_content_view_->info(result.msg());
            saveImage();
            save_model_img_ = true;
        }
        else {
            algo_content_view_->error("执行失败: " + result.msg());
            // 即使返回NG，如果ResultType是OK（用于调试显示），也画图
            if (result.resultType() == ResultType::OK) {
                algo_content_view_->imageEditor()->setResultShapes(result.resultShapes());
            }
        }
    }
}

void HairyFabricView::clearResults() { algo_content_view_->imageEditor()->clearAllResult(); }

void HairyFabricView::setImageSourceProp() {
    page_ids_ = manager()->getPageIdsByName("imagegrab");
    setImageSourcePropEnums(page_ids_);
    algo_content_view_->propEditor()->updateEditorValue("image_source");
}

void HairyFabricView::setImageSource() {
    QString pageId = algo_content_view_->propEditor()->propObject()->enumData("image_source").toString();
    if (pageId != page_id_) {
        page_id_ = pageId;
        algo_content_view_->imageEditor()->clearRois();
        clearResults();
    }
    QImage img = manager()->getPageImageById(page_id_);
    algo_content_view_->imageEditor()->setImage(img);
}

void HairyFabricView::onPageChangedSlots(QString pageName) {
    if (pageName == "imagegrab") setImageSourceProp();
}

void HairyFabricView::onPropValueChanged(QString key, QVariant value) {
    if (key == "image_source") setImageSource();
}

void HairyFabricView::onPageActivated(const QVariant& data) {
    setImageSource();
}

void HairyFabricView::setImageSourcePropEnums(QStringList list) {
    QList<QPair<QString, QVariant>> enums;
    for (int i = 0; i < list.size(); i++) enums.append(qMakePair(QString("图像%1").arg(i + 1), list[i]));
    QMap<QString, QList<QPair<QString, QVariant>>> runParams;
    runParams["image_source"] = enums;
    algo_instance_->setRunParameters(runParams);
}

QJsonObject HairyFabricView::algoParameters() const {
    QJsonObject obj = property_editor_->getPropJson();
    obj["rois"] = algo_content_view_->imageEditor()->getRoiParameters();
    if (save_model_img_) obj["model_image"] = modelImgName();
    return obj;
}

void HairyFabricView::setAlgoParameters(const QJsonObject& obj) {
    if (obj.contains("model_image") && !obj["model_image"].isNull()) {
        QString imgPath = obj["model_image"].toString();
        QFileInfo fi(imgPath);
        if (fi.exists() && fi.isFile()) {
            QImage img(imgPath);
            if (!img.isNull()) {
                save_model_img_ = true;
                algo_content_view_->imageEditor()->setImage(img);
            }
        }
    }
    algo_content_view_->propEditor()->setPropValue(obj);
    algo_content_view_->imageEditor()->loadRoi(obj);
}

void HairyFabricView::saveImage() {
    const QImage& image = algo_content_view_->imageEditor()->getImage();
    if (!image.isNull()) {
        QString path = modelImgName();
        QFileInfo fi(path);
        QDir dir = fi.absoluteDir();
        if (!dir.exists()) dir.mkpath(".");
        image.save(path);
    }
}