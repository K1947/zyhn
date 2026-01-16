#include "zyhn_main_window.h"
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "utils.h"
#include "permission_macros.h"

ZyhnMainWindow::ZyhnMainWindow(IApplication* app, QWidget* parent)
    : QMainWindow(parent), log_panel_(nullptr),
    left_panel_(nullptr), run_panel_(nullptr)
    , top_panel_(nullptr), stack_widget_(nullptr)
{
    app_ = app;
    Utils::setStyleSheet(this, ":/zyhn_app/style/qss/main.qss");
    initMainWindow();
    initConnect();
}

ZyhnMainWindow::~ZyhnMainWindow()
{}

void ZyhnMainWindow::initMainWindow()
{
    log_panel_ = new LogPanel(this);
    run_panel_ = new RunPanel(app_, this);
    left_panel_ = new LeftPanel(app_, this);
    top_panel_ = new TopPanel(app_, this);
    stack_widget_ = new QStackedWidget(this);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->addWidget(top_panel_);
    vLayout->addWidget(run_panel_);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(left_panel_);

    QVBoxLayout* vLayout2 = new QVBoxLayout();
    vLayout2->setContentsMargins(0, 0, 0, 0);
    vLayout2->addWidget(stack_widget_);
    vLayout2->addWidget(log_panel_);
    vLayout2->setStretch(0, 3);
    vLayout2->setStretch(1, 1);
    hLayout->addLayout(vLayout2);

    vision_img_panel_ = new VisionImgPanel(app_, this);
    hLayout->addWidget(vision_img_panel_);
    hLayout->setStretch(1, 3);
    hLayout->setStretch(2, 4);

    vLayout->addLayout(hLayout);

    QWidget* widget = new QWidget();
    this->setCentralWidget(widget);
    this->centralWidget()->setLayout(vLayout);

    spc_widget_ = new SpcPanel(app_);
    vision_widget_ = new ZyhnVisionWidget(app_);
    control_widget_ = new ControlPanel(app_);   
    

    stack_widget_->addWidget(spc_widget_);
    stack_widget_->addWidget(control_widget_);
    stack_widget_->addWidget(vision_widget_);
}

void ZyhnMainWindow::initPluginWidget()
{
    zyhn_core::IPlugin* plugin = app_->findPluginById("comm_plugin");
    if (plugin) {
        communicate_widget_ = plugin->createWgt(app_);
        stack_widget_->addWidget(communicate_widget_);
    }

    plugin = app_->findPluginById("mes_plugin");
    if (plugin) {
        mes_widget_ = plugin->createWgt(app_);
        stack_widget_->addWidget(mes_widget_);
    }

    emit app_->loadPluginsComplete();
}

void ZyhnMainWindow::initConnect()
{
    connect(left_panel_, &LeftPanel::spcBtnClickSignals, this, &ZyhnMainWindow::spcBtnClicked);
    connect(left_panel_, &LeftPanel::controlBtnClickSignals, this, &ZyhnMainWindow::controlBtnClicked);
    connect(left_panel_, &LeftPanel::visionBtnClickSignals, this, &ZyhnMainWindow::visionBtnClicked);
    connect(left_panel_, &LeftPanel::communicateBtnClickSignals, this, &ZyhnMainWindow::communicateBtnClicked);
    connect(left_panel_, &LeftPanel::mesClickSignals, this, &ZyhnMainWindow::mesBtnClicked);
}

void ZyhnMainWindow::spcBtnClicked()
{
    stack_widget_->setCurrentIndex(0);
}

void ZyhnMainWindow::controlBtnClicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_ControlPanel);
    stack_widget_->setCurrentIndex(1);
}

void ZyhnMainWindow::visionBtnClicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_VisionPanel);
    stack_widget_->setCurrentIndex(2);
}

void ZyhnMainWindow::communicateBtnClicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_CommPanel);
    stack_widget_->setCurrentIndex(3);
}

void ZyhnMainWindow::mesBtnClicked()
{
    CHECK_PERMISSION_COMMON_MSG(P_MesPanel);
    stack_widget_->setCurrentIndex(4);
}