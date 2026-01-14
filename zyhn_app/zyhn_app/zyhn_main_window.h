#ifndef ZYHN_VISION_ZYHN_MAIN_WINDOW_H_
#define ZYHN_VISION_ZYHN_MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "log4qt/logger.h"
#include "log_panel.h"
#include "zyhn_core.h"
#include "top_panel.h"
#include "left_panel.h"
#include "run_panel.h"
#include <QStackedWidget>
#include "zyhn_spc_widget.h"
#include "zyhn_control.h"
#include "vision_img_panel.h"
#include "zyhn_vision_widget.h"

using namespace zyhn_core;
class ZyhnMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ZyhnMainWindow(IApplication *app, QWidget *parent = nullptr);
    ~ZyhnMainWindow();
    void initPluginWidget();
private:
	void setTitilePos();
    void initMainWindow();
    void initConnect();
private slots:
    void spcBtnClicked();
    void controlBtnClicked();
    void visionBtnClicked();
    void communicateBtnClicked();
    void mesBtnClicked();

private:
	LogPanel* log_panel_;
    RunPanel* run_panel_;
    TopPanel* top_panel_;
    LeftPanel* left_panel_;
    QStackedWidget* stack_widget_;
    IApplication* app_;

    ZyhnVisionWidget* vision_widget_;
    ControlPanel* control_widget_;
    QWidget* communicate_widget_;
    SpcPanel* spc_widget_;
    QWidget* mes_widget_;
    VisionImgPanel* vision_img_panel_;
};

#endif // !ZYHN_VISION_ZYHN_MAIN_WINDOW_H_
