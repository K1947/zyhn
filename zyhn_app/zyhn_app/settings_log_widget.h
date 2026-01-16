#ifndef SETTINGS_LOG_WIDGET_H
#define SETTINGS_LOG_WIDGET_H

#include "ui_settings_log_widget.h"
#include <QSettings>

class SettingsLogWidget: public QWidget
{
    Q_OBJECT
public:
    SettingsLogWidget(QWidget* parent = nullptr);
    ~SettingsLogWidget();
    void initLogInfo();
private slots:
    void on_btn_save_clicked();
    void on_btn_select_dir_clicked();
private:
    Ui::settings_log_widget ui_;
    QSettings* log_settings_;
};

#endif // !SETTINGS_LOG_WIDGET_H