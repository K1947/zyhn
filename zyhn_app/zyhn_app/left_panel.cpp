#include "left_panel.h"
#include "utils.h"
#include "selectable_widget.h"
#include "user_login_dlg.h"
#include <QMessageBox>

LeftPanel::LeftPanel(IApplication* app, QWidget* parent) :QWidget(parent), app_(app), systemMonitor_(nullptr)
{
    ui_.setupUi(this);
    this->setObjectName("left_panel");
    this->setAttribute(Qt::WA_StyledBackground, true);
    zyhn_core::Utils::setStyleSheet(this, ":/zyhn_app/style/qss/left_panel.qss");

    setupSelectWidget();
    setupSystemMonitor();

    ui_.wgt_user->installEventFilter(this);
    ui_.label_user_permission->setVisible(false);
    ui_.wgt_user->setContextMenuPolicy(Qt::CustomContextMenu);
    user_menu_ = new QMenu(ui_.wgt_user);
    user_menu_->addAction("退出登录", this, &LeftPanel::onlogOutMenuClicked);

    connect(ui_.wgt_user, &QWidget::customContextMenuRequested, this, &LeftPanel::showContextMenu);
    connect(app_->userManager(), &IUserManager::userLoggedIn, this, &LeftPanel::onUserLoginedSlots);
    connect(app_, &IApplication::loadPluginsComplete, this, &LeftPanel::loadPluginCompleteSlots);
}

LeftPanel::~LeftPanel()
{
    if (systemMonitor_) {
        systemMonitor_->stopMonitoring();
        delete systemMonitor_;
        systemMonitor_ = nullptr;
    }
}

void LeftPanel::setupSelectWidget()
{
    SelectableWidget* wgt_spc = qobject_cast<SelectableWidget*>(ui_.wgt_spc);
    SelectableWidget* wgt_control = qobject_cast<SelectableWidget*>(ui_.wgt_control);
    SelectableWidget* wgt_vision = qobject_cast<SelectableWidget*>(ui_.wgt_vision);
    SelectableWidget* wgt_communate = qobject_cast<SelectableWidget*>(ui_.wgt_communate);
    SelectableWidget* wgt_mes = qobject_cast<SelectableWidget*>(ui_.wgt_mes);

    if (wgt_spc && wgt_control && wgt_vision && wgt_communate) {
        SelectableWidget::addToGroup(wgt_spc, "options");
        SelectableWidget::addToGroup(wgt_control, "options");
        SelectableWidget::addToGroup(wgt_vision, "options");
        SelectableWidget::addToGroup(wgt_communate, "options");
        SelectableWidget::addToGroup(wgt_mes, "options");

        connect(wgt_spc, &SelectableWidget::clicked, this, &LeftPanel::spcBtnClickSignals);
        connect(wgt_control, &SelectableWidget::clicked, this, &LeftPanel::controlBtnClickSignals);
        connect(wgt_vision, &SelectableWidget::clicked, this, &LeftPanel::visionBtnClickSignals);
        connect(wgt_communate, &SelectableWidget::clicked, this, &LeftPanel::communicateBtnClickSignals);
        connect(wgt_mes, &SelectableWidget::clicked, this, &LeftPanel::mesClickSignals);

        wgt_spc->setChecked(true);
    }
}

bool LeftPanel::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui_.wgt_user && event->type() == QEvent::MouseButtonRelease) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            if (app_->userManager()->isLoggedIn()) {
                return true;
            }

            // 打开用户登录对话框
            UserLoginDlg* dlg = new UserLoginDlg(app_);
            if (dlg->exec() == QDialog::Accepted) {
                if (dlg->isLoginSuccessful()) {
                    UserInfo user = dlg->getLoggedInUser();
                    qDebug() << "用户登录成功:" << user.username << "角色:" << user.role;
                    ui_.label_user_name->setText(user.username);
                    ui_.label_user_permission->setVisible(true);
                    ui_.label_user_permission->setText(user.realName);
                }
            }
            dlg->deleteLater();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void LeftPanel::setupSystemMonitor()
{
    systemMonitor_ = new SystemMonitor(this);
    connect(systemMonitor_, &SystemMonitor::systemInfoUpdated, 
            this, &LeftPanel::onSystemInfoUpdated);
    connect(systemMonitor_, &SystemMonitor::gpuInfoUpdated, this, &LeftPanel::onGpuInfoUpdated);
    systemMonitor_->startMonitoring(1000);
}

void LeftPanel::onSystemInfoUpdated(double cpuUsage, double memoryUsage, const QString& currentTime)
{
    ui_.label_cpu->setText(QString("%1%").arg(cpuUsage, 0, 'f', 1));
    ui_.label_memory->setText(QString("%1%").arg(memoryUsage, 0, 'f', 1));
    ui_.label_time->setText(currentTime);
}

void LeftPanel::onGpuInfoUpdated(int gpuUsage)
{
    ui_.label_gpu->setText(QString("%1%").arg(gpuUsage));
}

void LeftPanel::onlogOutMenuClicked()
{
    int ret = QMessageBox::warning(this, "提示", "确定退出登录吗?", QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        app_->userManager()->logout();
        ui_.label_user_name->setText("未登录");
        ui_.label_user_permission->setVisible(false);
    }
}

void LeftPanel::showContextMenu(const QPoint& pos)
{
    if (app_->userManager()->isLoggedIn()) {
        user_menu_->exec(ui_.wgt_user->mapToGlobal(pos));
    }
}

void LeftPanel::onUserLoginedSlots(int uid)
{
   UserInfo userinfo = app_->userManager()->getCurrentUser();
   ui_.label_user_name->setText(userinfo.username);
   ui_.label_user_permission->setVisible(true);
   ui_.label_user_permission->setText(userinfo.realName);
}

void LeftPanel::loadPluginCompleteSlots()
{
    if (!app_->findPluginById("comm_plugin")) {
        ui_.wgt_communate->setVisible(false);
    }
    if (!app_->findPluginById("mes_plugin")) {
        ui_.wgt_mes->setVisible(false);
    }
}