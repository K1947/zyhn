#ifndef ADD_USER_DLG_H
#define ADD_USER_DLG_H
#include "basedlg.h"
#include "ui_add_user_dlg.h"
#include "zyhn_core.h"

using namespace zyhn_core;
class AddUserDlg: public BaseDlg
{
    Q_OBJECT
public:
    AddUserDlg(IApplication* app, QWidget* parent = nullptr);
    ~AddUserDlg();
private:
    void initCombox();
private slots:
    void on_btn_save_clicked();
private:
    Ui::add_user_dlg ui_;
    IApplication* app_;
};

#endif

