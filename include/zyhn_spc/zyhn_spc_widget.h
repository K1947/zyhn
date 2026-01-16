#ifndef ZYHN_SPC_WIDGET_H
#define ZYHN_SPC_WIDGET_H

#include "zyhn_spc_global.h"
#include <QWidget>
#include "zyhn_core.h"
#include "spc_types.h"
#include <QStandardItemModel>

namespace Ui {
    class spc_widget;
}

class ZYHN_SPC_EXPORT SpcPanel: public QWidget
{
    Q_OBJECT
public:
    SpcPanel(zyhn_core::IApplication*app, QWidget* parent = nullptr);
    ~SpcPanel();
public slots:
    void clearResult();
private slots:
    void onProductSwitched();
    void newPointAdded(SpcMeasurement data);
    void statisticUpdate(QString stationId, SpcStatistics stats);
private:
    Ui::spc_widget *ui_;
    zyhn_core::IApplication* app_;
    QStandardItemModel* model_;
};

#endif // !ZYHN_SPC_WIDGET_H