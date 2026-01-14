#include "settings_basic_widget.h"
#include <QFileDialog>

SettingsBasicWidget::SettingsBasicWidget(zyhn_core::IApplication* app, QWidget* parent) : QWidget(parent),app_(app)
{
    ui_.setupUi(this);
    ui_.lineEdit_appname->setText(app_->settings()->value("general\\app_name", "视觉检测系统").toString());
    ui_.lineEdit_grap_image_path->setText(app_->settings()->value("general\\grab_img_path", "").toString());
    ui_.lineEdit_logo_path->setText(app_->settings()->value("general\\app_logo", "").toString());
}

SettingsBasicWidget::~SettingsBasicWidget()
{

}

void SettingsBasicWidget::on_btn_save_clicked()
{
    if (!ui_.lineEdit_appname->text().isEmpty()) {
        emit appNameChanged(ui_.lineEdit_appname->text());
    }

    if (!ui_.lineEdit_grap_image_path->text().isEmpty()) {
        app_->settings()->setValue("general\\grab_img_path", ui_.lineEdit_grap_image_path->text());
    }

    if (!ui_.lineEdit_logo_path->text().isEmpty()) {
        emit appLogoChanged(ui_.lineEdit_logo_path->text());
    }
}

void SettingsBasicWidget::on_btn_logo_path_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
        "选择logo文件",
        QDir::homePath(),
        "Images (*.png *.jpg);;All Files (*)");

    if (file.isEmpty()) {
        return;
    }

    ui_.lineEdit_logo_path->setText(file);
}

void SettingsBasicWidget::on_btn_grab_img_path_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择截图保存目录", QDir::homePath());

    if (dir.isEmpty()) {
        return;
    }

    ui_.lineEdit_grap_image_path->setText(dir);
}