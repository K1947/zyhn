#ifndef SETTINGS_BASIC_WIDGET_H
#define SETTINGS_BASIC_WIDGET_H
#include "ui_settings_basic_widget.h"
#include "zyhn_core.h"

class SettingsBasicWidget: public QWidget
{
    Q_OBJECT
public:
    SettingsBasicWidget(zyhn_core::IApplication* app, QWidget* parent = nullptr);
    ~SettingsBasicWidget();
private slots:
    void on_btn_save_clicked();
    void on_btn_logo_path_clicked();
    void on_btn_grab_img_path_clicked();
signals:
    void appNameChanged(QString);
    void appLogoChanged(QString);
private:
    Ui::settings_basic_widget ui_;
    zyhn_core::IApplication* app_;
};


#endif
