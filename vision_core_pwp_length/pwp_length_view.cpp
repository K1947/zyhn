#include "pwp_length_view.h"
#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>
#include "algo_page_manager.h"
#include "algorithm_manager.h"

PwpLengthView::PwpLengthView(AlgoPageManager* pageMgr, QString type, QWidget* parent) :
    PageBase(pageMgr, type, parent), page_id_(""), save_model_img_(false)
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

    connect(algo_content_view_->propEditor(), &PropertyEditorWidget::onBtnCliclkSignals, this, &PwpLengthView::onPropBtnClickedSlots);
    connect(manager(), &AlgoPageManager::pageAdded, this, &PwpLengthView::onPageChangedSlots);
    connect(manager(), &AlgoPageManager::pageRemoved, this, &PwpLengthView::onPageChangedSlots);
    connect(algo_content_view_->propEditor()->propObject(), &ConfigurableObject::propertyChanged, this, &PwpLengthView::onPropValueChanged);
}

PwpLengthView::~PwpLengthView()
{
    if (algo_instance_) delete algo_instance_;
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

            //测试成功后，自动保存当前图像为模板
            saveImage();
            save_model_img_ = true; // 标记已保存
            algo_content_view_->debug("模板图像已更新");
        }
        else {
            algo_content_view_->error("执行失败: " + result.msg());
        }
    }
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

// 【关键实现】：保存参数时，把模板图像路径也写进去
QJsonObject PwpLengthView::algoParameters() const
{
    QJsonObject obj = property_editor_->getPropJson();
    // 保存 ROI 信息
    obj["rois"] = algo_content_view_->imageEditor()->getRoiParameters();

    // 如果有新的模板图像，保存它的路径
    // modelImgName() 是基类 PageBase 提供的函数，它会生成一个基于算法ID的标准路径
    if (save_model_img_) {
        obj["model_image"] = modelImgName();
    }
    return obj;
}

// 【关键实现】：加载参数时，如果有模板图像，就显示出来
void PwpLengthView::setAlgoParameters(const QJsonObject& obj)
{
    // 检查参数里有没有 model_image 字段
    if (!obj["model_image"].isNull()) {
        QString imgPath = obj["model_image"].toString();
        QImage img(imgPath);
        if (!img.isNull()) {
            save_model_img_ = true;
            // 将模板图像设置到编辑器中显示
            algo_content_view_->imageEditor()->setImage(img);
            qDebug() << "[PWP View] 加载模板图像:" << imgPath;
        }
    }

    // 恢复参数值
    algo_content_view_->propEditor()->setPropValue(obj);
    // 恢复 ROI
    algo_content_view_->imageEditor()->loadRoi(obj);
}

// 【关键实现】：具体的保存逻辑
void PwpLengthView::saveImage()
{
    QImage image = algo_content_view_->imageEditor()->getImage();
    if (!image.isNull()) {
        // 保存到标准路径，文件名通常是 "算法名_ID.png"
        QString path = modelImgName();
        bool success = image.save(path);
        if (success) {
            qDebug() << "[PWP View] 模板图像保存成功:" << path;
        }
        else {
            qDebug() << "[PWP View] 模板图像保存失败:" << path;
        }
    }
}