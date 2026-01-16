#include "top_panel.h"
#include "log4qt/logger.h"
#include "utils.h"
#include "settings_dlg.h"
#include "permission_macros.h"
#include "about_dlg.h"
#include "plugins_dlg.h"
#include "toast.h"
#include <QDateTime>
#include <QMessageBox>
#include "product_list_dlg.h"

TopPanel::TopPanel(zyhn_core::IApplication *app, QWidget* parent) :QWidget(parent), app_(app),seconds_(0)
{
    ui_.setupUi(this);
    this->setObjectName("top_panel");
    this->setAttribute(Qt::WA_StyledBackground, true);
    zyhn_core::Utils::setStyleSheet(this, ":/zyhn_app/style/qss/top_panel.qss");
    ui_.label_logo->setAlignment(Qt::AlignCenter);

    timer_ = new QTimer();
    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &TopPanel::onTimerSlots);
    QString appName = app_->settings()->value("general\\app_name", "视觉检测系统").toString();
    ui_.label_title->setText(appName);

    QString logoPath = app_->settings()->value("general\\app_logo", ":/zyhn_app/style/img/logo.png").toString();
    QPixmap pixmap(logoPath);
    ui_.label_logo->setVisible(false);
    // ui_.label_logo->setPixmap(pixmap.scaled(QSize(175, 40), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui_.btn_tool->setContextMenuPolicy(Qt::CustomContextMenu);
    tool_menu_ = new QMenu(this);
    tool_menu_->addAction("截图", this, &TopPanel::onActionGrabImage);
    tool_menu_->addAction("相机标定", this, &TopPanel::onActionCameraCaliber);
}

TopPanel::~TopPanel()
{
    tool_menu_->deleteLater();
    if (timer_) {
        timer_->stop();
    }
}

void TopPanel::setTitilePos()
{
    int btn_about_pos = ui_.btn_about->geometry().x();
    if (btn_about_pos <= 0)
        return;

    int title_width = ui_.label_title->geometry().width();
    int width = this->geometry().width();
    int center = width / 2 - btn_about_pos - title_width;
    ui_.head_fix_spacer->changeSize(center, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    this->layout()->invalidate();
    this->layout()->activate();
}

void TopPanel::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    setTitilePos();
    timer_->start();
}

void TopPanel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    setTitilePos();
}

void TopPanel::onTimerSlots()
{
    seconds_++;
    int day = seconds_ / 86400;
    int hour = (seconds_ % 86400) / 3600;
    int minute = (seconds_ % 3600) / 60;
    int second = seconds_ % 60;

    ui_.label_run_time->setText(QString("%1天%2小时%3分%4秒").arg(day, 2, 10, QChar('0')).arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0')));
}

void TopPanel::on_btn_product_clicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_ProductView);
    zyhn_product::ProductListDlg* dlg = new zyhn_product::ProductListDlg(app_);
    dlg->exec();

    dlg->deleteLater();
}

void TopPanel::on_btn_device_clicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_DeviceView);
    zyhn_core::IPlugin* plugin = app_->findPluginById("device_plugin");
    if (plugin) {
        QDialog* dlg = plugin->createDlg(app_);
        dlg->exec();

        dlg->deleteLater();
    }
}

void TopPanel::on_btn_setting_clicked()
{
    SettingsDlg* dlg = new SettingsDlg(app_);
    connect(dlg, &SettingsDlg::appNameChanged, this, &TopPanel::onAppNameChanged);
    connect(dlg, &SettingsDlg::appLogoChanged, this, &TopPanel::onAppLogoChanged);
    dlg->exec();

    dlg->deleteLater();
}

void TopPanel::on_btn_plugins_clicked()
{
    PluginsDlg* dlg = new PluginsDlg(app_);
    dlg->exec();

    dlg->deleteLater();
}

void TopPanel::on_btn_about_clicked()
{
    AboutDlg* dlg = new AboutDlg();
    dlg->exec();

    dlg->deleteLater();
}

void TopPanel::on_btn_exit_clicked()
{
    int ret = QMessageBox::warning(this, "提示", QString("确定退出%1吗").arg(ui_.label_title->text()), QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        QCoreApplication::quit();
    }
}

void TopPanel::onAppNameChanged(QString name)
{
    ui_.label_title->setText(name);
    app_->settings()->setValue("general\\app_name", name);
}

void TopPanel::onAppLogoChanged(QString name)
{
    QPixmap pixmap(name);
    ui_.label_logo->setPixmap(pixmap.scaled(ui_.label_logo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    app_->settings()->setValue("general\\app_logo", name);
}

void TopPanel::on_btn_tool_clicked()
{
    QPoint pos = ui_.btn_tool->pos();
    pos.setY(pos.y() + 40);
    tool_menu_->exec(this->mapToGlobal(pos));
}

void TopPanel::onActionGrabImage()
{
    QString filePath = app_->settings()->value("general\\grab_img_path", "").toString();
    if (filePath.isEmpty()) {
        Toast::warning("请先在“设置”中设置截图保存路径");
        return;
    }

    QPixmap pixmap = app_->mainWindow()->grab();
    QString filename = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    pixmap.save(filePath + QString("/%1.png").arg(filename));

    Toast::success("截图保存成功");
}

void TopPanel::onActionCameraCaliber()
{

}