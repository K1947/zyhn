#ifndef USER_LOGIN_DLG_H
#define USER_LOGIN_DLG_H

#include "ui_user_login_dlg.h"
#include "basedlg.h"
#include "zyhn_core.h"

using namespace zyhn_core;
class UserLoginDlg: public BaseDlg
{
    Q_OBJECT
public:
    UserLoginDlg(IApplication* app, QWidget* parent = nullptr);
    ~UserLoginDlg();
    
    UserInfo getLoggedInUser() const;
    bool isLoginSuccessful() const;

private slots:
    void on_btn_login_clicked();
    void on_btn_cancel_clicked();

private:
    Ui::user_login_dlg ui_;
    IApplication* app_;
    UserInfo loggedInUser_;
    bool loginSuccessful_;
};

#endif // !USER_LOGIN_DLG_H