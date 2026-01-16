#ifndef SETTINGS_DLG_H
#define SETTINGS_DLG_H
#include "ui_settings_dlg.h"
#include "basedlg.h"
#include "zyhn_core.h"
#include <QButtonGroup>
#include "settings_user_widget.h"
#include "settings_basic_widget.h"
#include "settings_log_widget.h"

using namespace zyhn_core;
class SettingsDlg: public BaseDlg
{
    Q_OBJECT
public:
    SettingsDlg(IApplication* app, QWidget* parent = nullptr);
    ~SettingsDlg();
private:
    void initBtnGroup();
    void initStackedWidget();
private slots:
    void onBtnGroupClicked(int id);
signals:
    void appNameChanged(QString);
    void appLogoChanged(QString);
private:
    Ui::settingsDlg ui_;
    IApplication* app_;
    QButtonGroup* btn_group_;
    SettingsUserWidget* settings_user_widget_;
    SettingsBasicWidget* settings_basic_widget_;
    SettingsLogWidget* settings_log_widget_;
};

#endif // !SETTINGS_DLG_H