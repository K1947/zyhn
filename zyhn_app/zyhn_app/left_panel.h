#ifndef LEFT_PANEL_H
#define LEFT_PANEL_H
#include "ui_left_panel.h"
#include "zyhn_core.h"
#include "system_monitor.h"
#include <QMenu>

using namespace zyhn_core;
class LeftPanel: public QWidget
{
    Q_OBJECT
public:
    LeftPanel(IApplication* app, QWidget* parent = nullptr);
    ~LeftPanel();
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
    void setupSelectWidget();
    void setupSystemMonitor();
    void onlogOutMenuClicked();

private slots:
    void onSystemInfoUpdated(double cpuUsage, double memoryUsage,const QString& currentTime);
    void onGpuInfoUpdated(int gpuUsage);
    void showContextMenu(const QPoint& pos);
    void onUserLoginedSlots(int uid);
    void loadPluginCompleteSlots();

signals:
    void communicateBtnClickSignals();
    void visionBtnClickSignals();
    void spcBtnClickSignals();
    void controlBtnClickSignals();
    void mesClickSignals();

private:
    Ui::left_panel ui_;
    IApplication* app_;
    SystemMonitor* systemMonitor_;
    QMenu* user_menu_;
};

#endif

