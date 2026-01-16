#include "user_login_dlg.h"
#include "utils.h"
#include <QMessageBox>
#include <QDebug>
#include "toast.h"

UserLoginDlg::UserLoginDlg(IApplication* app, QWidget* parent)
    : BaseDlg(parent), app_(app), loginSuccessful_(false)
{
    ui_.setupUi(ContentArea());
    setTitleText("用户登录");
    setResizable(false);
    Utils::setStyleSheet(this, ":/zyhn_app/style/qss/user_login.qss");
    resize(420, 220);

    QMetaObject::connectSlotsByName(this);
    ui_.lineEdit_user_name->setFocus();
}

UserLoginDlg::~UserLoginDlg()
{
}

UserInfo UserLoginDlg::getLoggedInUser() const
{
    return loggedInUser_;
}

bool UserLoginDlg::isLoginSuccessful() const
{
    return loginSuccessful_;
}

void UserLoginDlg::on_btn_login_clicked()
{
    QString username = ui_.lineEdit_user_name->text().trimmed();
    QString password = ui_.lineEdit_password->text();
    
    if (username.isEmpty()) {
        Toast::warning("请输入用户名");
        ui_.lineEdit_user_name->setFocus();
        return;
    }
    
    if (password.isEmpty()) {
        Toast::warning("请输入密码");
        ui_.lineEdit_password->setFocus();
        return;
    }
    
    if (app_->userManager()->login(username, password)) {
        loggedInUser_ = app_->userManager()->getCurrentUser();
        loginSuccessful_ = true;
        qDebug() << "用户登录成功:" << username;
        accept();
    } else {
        Toast::warning("用户名或密码错误");
        ui_.lineEdit_password->clear();
        ui_.lineEdit_password->setFocus();
    }
}

void UserLoginDlg::on_btn_cancel_clicked()
{
    reject();
}