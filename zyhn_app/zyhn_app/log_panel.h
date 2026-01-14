#ifndef LOG_PANEL_H
#define LOG_PANEL_H
#include <QWidget>
#include "ui_log_panel.h"

class LogPanel: public QWidget
{
    Q_OBJECT
public:
	LogPanel(QWidget* parent = nullptr);
	~LogPanel() override = default;
private:
	void InitView();
private slots:
	void on_btn_clear_log_clicked();
private:
	Ui::log_panel* ui_;
}; 

#endif // !LOG_PANEL_H