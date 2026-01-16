#pragma once
#include "ui_settings_user_widget.h"
#include "zyhn_core.h"

using namespace zyhn_core;
class SettingsUserWidget: public QWidget
{
    Q_OBJECT
public:
    SettingsUserWidget(IApplication* app, QWidget* parent = nullptr);
    ~SettingsUserWidget();
private:
    void initTableWidget();
    void loadUserList();
    QWidget* createOperateBtn(int uid);
private slots:
    void on_btn_add_user_clicked();
private:
    Ui::user_list_widget ui_;
    IApplication* app_;
};

