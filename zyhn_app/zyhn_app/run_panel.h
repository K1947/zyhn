#ifndef RUN_PANEL_H
#define RUN_PANEL_H

#include "ui_run_panel.h"
#include "zyhn_core.h"

using namespace zyhn_core;
class RunPanel : public QWidget
{
    Q_OBJECT
public:
    RunPanel(IApplication* app, QWidget* parent = nullptr);
    ~RunPanel();
private slots:
    void onProductListChanged();
    void onComboBoxProductIndexChanged(int index);
    void on_btn_run_clicked();
    void on_btn_stop_clicked();
    void on_btn_reset_clicked();
private:
    Ui::run_panel ui_;
    IApplication* app_;
    QString combo_cur_text_;

signals:
};

#endif