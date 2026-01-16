#include "log_panel.h"
#include <QFile>
#include <QTimer>
#include <Log4Qt/level.h>
#include "utils.h"

LogPanel::LogPanel(QWidget* parent) : QWidget(parent), ui_(new Ui::log_panel)
{
    ui_->setupUi(this);
    this->setObjectName("log_panel");
    this->setAttribute(Qt::WA_StyledBackground, true);
    zyhn_core::Utils::setStyleSheet(this, ":/zyhn_app/style/qss/log_panel.qss");
	InitView();
}

void LogPanel::InitView()
{    
    ui_->comboBox_level->addItem("All", Log4Qt::Level::ALL_INT);
    ui_->comboBox_level->addItem("Debug", Log4Qt::Level::DEBUG_INT);
    ui_->comboBox_level->addItem("Info", Log4Qt::Level::INFO_INT);
    ui_->comboBox_level->addItem("Warn", Log4Qt::Level::WARN_INT);
    ui_->comboBox_level->addItem("Error", Log4Qt::Level::ERROR_INT);
    ui_->comboBox_level->addItem("Fatal", Log4Qt::Level::FATAL_INT);
    ui_->comboBox_level->setCurrentIndex(0);

    connect(ui_->comboBox_level, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](int index) {
            int level = ui_->comboBox_level->currentData().toInt();
            ui_->textBrowser_log->showLogByLevel(level);
        });

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(300);
    connect(ui_->lineEdit_keyvalue, &QLineEdit::textChanged, [=]() {
        timer->start();
        });
    connect(timer, &QTimer::timeout, [=]() {
        QString text = ui_->lineEdit_keyvalue->text();
        ui_->textBrowser_log->filterLog(text);
        });
}

void LogPanel::on_btn_clear_log_clicked()
{
    ui_->textBrowser_log->clearAllLog();
}