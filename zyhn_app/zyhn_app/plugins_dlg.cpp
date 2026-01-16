#include "plugins_dlg.h"
#include <QFileInfo>

PluginsDlg::PluginsDlg(zyhn_core::IApplication* app, QWidget* parent) : BaseDlg(parent), app_(app)
{
    ui_.setupUi(ContentArea());
    setTitleText("插件管理");
    resize(600, 400);

    initTreeView();
    setTreeViewData();
}

PluginsDlg::~PluginsDlg()


{

}

void PluginsDlg::initTreeView()
{
    m_model = new QStandardItemModel(0, 6, this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("名称"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("是否可用"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("详情"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("作者"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("版本"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("文件名称"));
    ui_.treeView->setModel(m_model);
    ui_.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.treeView->setItemsExpandable(true);
    ui_.treeView->setRootIsDecorated(false);
    ui_.treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
}

void PluginsDlg::setTreeViewData()
{
    foreach(zyhn_core::IPluginFactory* factory, app_->pluginManager()->getPluginList())
    {
        zyhn_core::PluginInfo* info = factory->info();
        QList<QStandardItem*> items;
        items.append(new QStandardItem(info->name()));
        QStandardItem* load = new QStandardItem();
        load->setCheckable(true);
        load->setData(info->id());
        if (info->isMustLoad()) {
            load->setEnabled(false);
        }

        bool b = app_->settings()->value(QString("plugin\\%1_load").arg(info->id()), true).toBool();
        if (b) {
            load->setCheckState(Qt::Checked);
        }
        else {
            load->setCheckState(Qt::Unchecked);
        }

        items.append(load);
        items.append(new QStandardItem(info->info()));
        items.append(new QStandardItem(info->author()));
        items.append(new QStandardItem(info->ver()));
        items.append(new QStandardItem(QFileInfo(info->filePath()).fileName()));
        items.at(5)->setToolTip(info->filePath());
        m_model->appendRow(items);
    }
}

void PluginsDlg::itemChanged(QStandardItem* item)
{
    if (item && item->isCheckable()) {
        QString id = item->data(Qt::UserRole + 1).toString();
        app_->settings()->setValue(QString("plugin\\%1_load").arg(id), item->checkState() == Qt::Checked);
    }
}