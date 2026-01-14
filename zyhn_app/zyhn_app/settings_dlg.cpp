#include "settings_dlg.h"
#include "utils.h"

SettingsDlg::SettingsDlg(IApplication* app, QWidget* parent) : BaseDlg(parent), app_(app)
{
    ui_.setupUi(ContentArea());

    setTitleText("设置");
    resize(800, 600);
    Utils::setStyleSheet(this, ":/zyhn_app/style/qss/settings_dialog.qss");

    QMetaObject::connectSlotsByName(this);

    btn_group_ = new QButtonGroup(this);
    initBtnGroup();
    initStackedWidget();
}

SettingsDlg::~SettingsDlg()
{

}

void SettingsDlg::initBtnGroup()
{
    btn_group_->addButton(ui_.btn_user, 0);
    btn_group_->addButton(ui_.btn_setting, 1);
    btn_group_->addButton(ui_.btn_log, 2);
    btn_group_->addButton(ui_.btn_system, 3);

    connect(btn_group_, &QButtonGroup::idClicked, this, &SettingsDlg::onBtnGroupClicked);
    ui_.btn_user->setChecked(true);
}

void SettingsDlg::initStackedWidget()
{
    while (ui_.stackedWidget->count() > 0)
    {
        QWidget* widget = ui_.stackedWidget->widget(0);
        ui_.stackedWidget->removeWidget(widget);
        delete widget;
    }

    settings_user_widget_ = new SettingsUserWidget(app_, ui_.stackedWidget);
    settings_basic_widget_ = new SettingsBasicWidget(app_, ui_.stackedWidget);
    settings_log_widget_ = new SettingsLogWidget(ui_.stackedWidget);
    ui_.stackedWidget->addWidget(settings_user_widget_);
    ui_.stackedWidget->addWidget(settings_basic_widget_);
    ui_.stackedWidget->addWidget(settings_log_widget_);
    connect(settings_basic_widget_, &SettingsBasicWidget::appNameChanged, this, &SettingsDlg::appNameChanged);
    connect(settings_basic_widget_, &SettingsBasicWidget::appLogoChanged, this, &SettingsDlg::appLogoChanged);
}

void SettingsDlg::onBtnGroupClicked(int id)
{
    int curIdx = ui_.stackedWidget->currentIndex();
    if (curIdx != id && id < 3) {
        ui_.stackedWidget->setCurrentIndex(id);
    }
}