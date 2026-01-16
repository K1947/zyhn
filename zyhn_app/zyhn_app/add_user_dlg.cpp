#include "add_user_dlg.h"
#include "toast.h"

AddUserDlg::AddUserDlg(IApplication* app, QWidget* parent) :BaseDlg(parent), app_(app)
{
    ui_.setupUi(ContentArea());
    setTitleText("新增用户");
    setResizable(false);
    resize(500, 400);

    QMetaObject::connectSlotsByName(this);
    initCombox();
}

AddUserDlg::~AddUserDlg()
{

}

void AddUserDlg::initCombox()
{
    ui_.comboBox->addItem("管理员");
    ui_.comboBox->addItem("操作员");
    ui_.comboBox->setCurrentIndex(0);
}

void AddUserDlg::on_btn_save_clicked()
{
    if (ui_.lineEdit_name->text().isEmpty()) {
        Toast::warning("请输入用户名");
        return;
    }

    if (ui_.lineEdit_pwd->text().isEmpty() || ui_.lineEdit_pwd_2->text().isEmpty()) {
        Toast::warning("请输入密码");
        return;
    }

    if (ui_.lineEdit_pwd->text() != ui_.lineEdit_pwd_2->text()) {
        Toast::warning("确认密码不正确");
        return;
    }

    if (ui_.lineEdit_pwd->text().length() > 6) {
        Toast::warning("密码长度不能超过6位");
        return;
    }

    UserInfo userinfo;
    userinfo.username = ui_.lineEdit_name->text();
    userinfo.realName = ui_.comboBox->currentText();
    userinfo.password = ui_.lineEdit_pwd->text();
    userinfo.role = ui_.comboBox->currentIndex() == 0 ? UserRole::UR_Admin : UserRole::UR_Operator;

    bool ret = app_->userManager()->addUser(userinfo);
    if (!ret) {
        Toast::warning("创建用户失败");
        return;
    }
    accept();
}