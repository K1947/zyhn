#ifndef PLUGINS_DLG_H
#define PLUGINS_DLG_H

#include "basedlg.h"
#include "ui_plugins_dlg.h"
#include <QStandardItemModel>
#include "zyhn_core.h"

class PluginsDlg: public BaseDlg
{
    Q_OBJECT
public:
    PluginsDlg(zyhn_core::IApplication* app, QWidget* parent = nullptr);
    ~PluginsDlg();
private:
    void initTreeView();
    void setTreeViewData();
private slots:
    void itemChanged(QStandardItem*);
private:
    Ui::plugins_dlg ui_;
    QStandardItemModel* m_model;
    zyhn_core::IApplication* app_;
};

#endif // !PLUGINS_DLG_H