#ifndef ZYHN_CONTROL_H
#define ZYHN_CONTROL_H

#include "zyhn_control_global.h"
#include "zyhn_core.h"
#include <QWidget>

namespace Ui {
    class control_panel;
}

class ZYHN_CONTROL_EXPORT ControlPanel : public QWidget
{
    Q_OBJECT
public:
    ControlPanel(zyhn_core::IApplication* app, QWidget* parent = nullptr);
    ~ControlPanel();
private slots:
    void onProductSwitched();
private:
    Ui::control_panel *ui_;
    zyhn_core::IApplication* app_;
};

#endif
