#include "settings_user_widget.h"
#include <QPushButton>
#include <QMessageBox>
#include "add_user_dlg.h"
#include "permission_macros.h"

SettingsUserWidget::SettingsUserWidget(IApplication* app, QWidget* parent) : QWidget(parent), app_(app)
{
    ui_.setupUi(this);
    initTableWidget();
    loadUserList();
}

SettingsUserWidget::~SettingsUserWidget()
{

}

void SettingsUserWidget::initTableWidget()
{
    QStringList headers;
    headers << "ID" << "姓名" << "角色" << "创建时间" << "操作";
    ui_.tableWidget_user->setColumnCount(headers.size());
    ui_.tableWidget_user->setHorizontalHeaderLabels(headers);
    QVector<int> vecHeadWidth({ 80, 120, 120, 120, 120 });
    for (int i = 0; i < vecHeadWidth.size(); i++) {
        ui_.tableWidget_user->setColumnWidth(i, vecHeadWidth[i]);
    }
    ui_.tableWidget_user->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.tableWidget_user->horizontalHeader()->setStretchLastSection(true);
    ui_.tableWidget_user->verticalHeader()->setVisible(false);
    ui_.tableWidget_user->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_.tableWidget_user->setSelectionMode(QAbstractItemView::SingleSelection);
}

QWidget* SettingsUserWidget::createOperateBtn(int uid)
{
    QWidget* w = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addSpacing(4);
    layout->addStretch();

    auto newBtn = [&](const QString& res) {
        QPushButton* btn = new QPushButton();
        btn->setIcon(QIcon(res));
        btn->setFlat(true);
        btn->setIconSize(QSize(24, 24));
        btn->setStyleSheet("{border: none;}");
        layout->addWidget(btn);
        return btn;
        };

    QPushButton* btn = newBtn(":/zyhn_product/style/product_del.png");
    btn->setProperty("uid", uid);

    layout->addStretch();
    w->setLayout(layout);

    connect(btn, &QPushButton::clicked, this, [this, btn] {
        CHECK_PERMISSION_COMMON_MSG(P_UserOperate);
        int uid = btn->property("uid").toInt();
        int ret = QMessageBox::warning(this, "提示", "确定删除该用户吗？", QMessageBox::Yes | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
            app_->userManager()->removeUser(uid);

            loadUserList();
        }
    });

    return w;
}

void SettingsUserWidget::loadUserList()
{
    app_->userManager()->refreshUserList();
    ui_.tableWidget_user->setRowCount(0);
    QVector<UserInfo> list = app_->userManager()->userList();
    ui_.tableWidget_user->setRowCount(list.size());
    for (int i = 0; i < list.count(); i++) {
        if (list[i].role == UserRole::UR_SuperAdmin) {
            continue;
        }

        QTableWidgetItem* item = new QTableWidgetItem(QString::number(list[i].id));
        item->setTextAlignment(Qt::AlignCenter);
        ui_.tableWidget_user->setItem(i, 0, item);
        item = new QTableWidgetItem(list[i].username);
        item->setTextAlignment(Qt::AlignCenter);
        ui_.tableWidget_user->setItem(i, 1, item);
        item = new QTableWidgetItem(list[i].realName);
        item->setTextAlignment(Qt::AlignCenter);
        ui_.tableWidget_user->setItem(i, 2, item);
        item = new QTableWidgetItem(list[i].createTime);
        ui_.tableWidget_user->setItem(i, 3, item);

        ui_.tableWidget_user->setCellWidget(i, 4, createOperateBtn(list[i].id));
    }
}

void SettingsUserWidget::on_btn_add_user_clicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_UserOperate);
    AddUserDlg* dlg = new AddUserDlg(app_);
    int ret = dlg->exec();

    if (ret == QDialog::Accepted) {
        loadUserList();
    }

    dlg->deleteLater();
}