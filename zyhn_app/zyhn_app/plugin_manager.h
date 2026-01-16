#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H
#include <QObject>
#include "zyhn_core.h"

using namespace zyhn_core;

class PluginManager: public IPluginManager
{
    Q_OBJECT
public:
    PluginManager(QObject* parent = nullptr);
    ~PluginManager() override = default;
    virtual void loadPlugins(const QString& dir) override;
    virtual QList<IPluginFactory*> getPluginList() const override;
private:
    QList<IPluginFactory*> plugin_list_;
    bool is_loaded_ = false;
};

#endif // !PLUGIN_MANAGER_H
