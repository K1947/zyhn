#ifndef TOP_PANEL_H
#define TOP_PANEL_H

#include "ui_top_panel.h"
#include "zyhn_core.h"
#include <QMenu>

using namespace zyhn_core;
class TopPanel: public QWidget
{
    Q_OBJECT
public:
    TopPanel(zyhn_core::IApplication *app, QWidget* parent = nullptr);
    ~TopPanel();
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
private:
    void setTitilePos();
private slots:
    void on_btn_product_clicked();
    void on_btn_device_clicked();
    void on_btn_setting_clicked();
    void on_btn_plugins_clicked();
    void on_btn_about_clicked();
    void on_btn_tool_clicked();
    void on_btn_exit_clicked();
    void onAppNameChanged(QString);
    void onAppLogoChanged(QString);
    void onActionGrabImage();
    void onActionCameraCaliber();  // 相机标定
    void onTimerSlots();
private:
    Ui::top_panel ui_;
    zyhn_core::IApplication* app_;
    QMenu* tool_menu_;
    QTimer* timer_;
    int seconds_;
};

#endif

