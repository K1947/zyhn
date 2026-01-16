#include "run_panel.h"
#include "log4qt/logger.h"
#include <QJsonArray>
#include <QJsonObject>
#include "utils.h"
#include <QMessageBox>
#include "loading_manager.h"
// #include "communication_manager.h"

RunPanel::RunPanel(zyhn_core::IApplication* app, QWidget* parent) :QWidget(parent), app_(app), combo_cur_text_("")
{
    ui_.setupUi(this);
    this->setObjectName("run_panel");
    this->setAttribute(Qt::WA_StyledBackground, true);

    Utils::setStyleSheet(this, ":/zyhn_app/style/qss/run_panel.qss");

    connect(app_->productManager(), &IProductManager::productListChanged,
        this, &RunPanel::onProductListChanged);
    connect(app_->productManager(), &IProductManager::productListChanged,
        this, &RunPanel::onProductListChanged);
    connect(app_->productManager(), &IProductManager::productAdded,
        this, &RunPanel::onProductListChanged);
    connect(app_->productManager(), &IProductManager::productDeled,
        this, &RunPanel::onProductListChanged);
    connect(ui_.cmb_product, &QComboBox::currentIndexChanged, this, &RunPanel::onComboBoxProductIndexChanged);
}

RunPanel::~RunPanel()
{

}

void RunPanel::onProductListChanged()
{
    Log4Qt::Logger::logger("zyhn_vision")->info("产品列表已更新");

    if (!app_ || !app_->productManager()) {
        Log4Qt::Logger::logger("zyhn_vision")->warn("ProductManager not available");
        return;
    }

    QJsonArray productList = app_->productManager()->productList();

    ui_.cmb_product->blockSignals(true);
    ui_.cmb_product->clear();
    for (auto json : productList) {
        QJsonObject obj = json.toObject();
        ui_.cmb_product->addItem(obj["name"].toString(), obj["id"].toInt());
    }

    QJsonObject curInfo = app_->productManager()->currentProduct();
    if (!curInfo["name"].toString().isEmpty()) {
        ui_.cmb_product->setCurrentText(curInfo["name"].toString());
        combo_cur_text_ = curInfo["name"].toString();
    }
    ui_.cmb_product->blockSignals(false);
}

void RunPanel::onComboBoxProductIndexChanged(int index)
{
    int ret = QMessageBox::warning(this, "提示", "确定切换产品吗？", QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        QJsonObject info = app_->productManager()->findProductByName(ui_.cmb_product->currentText());
        if (info["id"].toInt() != -1) {
            app_->productManager()->setCurrentProduct(info["id"].toInt());
        }
        combo_cur_text_ = info["name"].toString();
        return;
    }
    else {
        ui_.cmb_product->blockSignals(true);
        ui_.cmb_product->setCurrentText(combo_cur_text_);
        ui_.cmb_product->blockSignals(false);
    }
}

void RunPanel::on_btn_run_clicked()
{
    //LoadingManager::instance().begin("开始运行，请稍等");
    //QTimer::singleShot(4000, this, [] {
    //    LoadingManager::instance().end();
    //});

    /*if (CommunicationManager::getInstance()->getConnectedCount() != CommunicationManager::getInstance()->getTotalCount())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认", "存在未连接的通信，确定要继续吗？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) 
        {
            CommunicationManager::getInstance()->startEventCycle();
        }
        else 
        {
            return;
        }
    }*/
}

void RunPanel::on_btn_stop_clicked()
{
    // CommunicationManager::getInstance()->stopEventCycle();
}

void RunPanel::on_btn_reset_clicked()
{
    // CommunicationManager::getInstance()->resetEventCycle();
}
