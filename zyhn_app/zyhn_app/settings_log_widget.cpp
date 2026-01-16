#include "settings_log_widget.h"
#include <log4Qt/propertyconfigurator.h>
#include <QFileDialog>
#include <QDesktopServices>

SettingsLogWidget::SettingsLogWidget(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    log_settings_ = new QSettings(qApp->applicationDirPath() + "/zyhn.log.config", QSettings::IniFormat);

    initLogInfo();
}

SettingsLogWidget::~SettingsLogWidget()
{

}

void SettingsLogWidget::initLogInfo()
{
    QString level = log_settings_->value("log4j.appender.daily.threshold", "ALL").toString();
    ui_.comboBox_level->setCurrentText(level);

    bool handleQtMessages = log_settings_->value("log4j.handleQtMessages", "true").toBool();
    ui_.checkBox_qtlog->setChecked(handleQtMessages);

    int keepdays = log_settings_->value("log4j.appender.daily.keepDays", "90").toInt();
    ui_.spinBox_keepdays->setValue(keepdays);

    ui_.label_file_path->setText(qApp->applicationDirPath() + "/log");
}

void SettingsLogWidget::on_btn_select_dir_clicked()
{
    QString path = qApp->applicationDirPath() + "/log";
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void SettingsLogWidget::on_btn_save_clicked()
{
    log_settings_->setValue("log4j.appender.daily.threshold", ui_.comboBox_level->currentText());
    log_settings_->setValue("log4j.handleQtMessages", ui_.checkBox_qtlog->isChecked());
    log_settings_->setValue("log4j.appender.daily.keepDays", ui_.spinBox_keepdays->value());
    log_settings_->setValue("logpath", ui_.label_file_path->text());
    Log4Qt::PropertyConfigurator::configure(qApp->applicationDirPath() + "/zyhn.log.config");
}