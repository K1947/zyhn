#include "plugin_manager.h"
#include <QPluginLoader>
#include <QDir>

PluginManager::PluginManager(QObject* parent)
{
}

void PluginManager::loadPlugins(const QString& dir)
{
    if (is_loaded_) {
        return;
    }
    is_loaded_ = true;

    QDir pluginsDir = dir;
    pluginsDir.setFilter(QDir::Files| QDir::NoSymLinks);

    QMap<QString, int> idxMap;
    QMap<QString, IPluginFactory*> pluginMap;
    foreach(QFileInfo info, pluginsDir.entryInfoList())
    {
        QPluginLoader loader(info.filePath());
        if (IPluginFactory* factory = qobject_cast<IPluginFactory*>(loader.instance())) {
            if (factory) {
                factory->setFilePath(info.filePath());
                idxMap.insert(factory->id(), 0);
                pluginMap.insert(factory->id(), factory);
            }
        }
    }

    if (idxMap.isEmpty()) {
        return;
    }

    foreach(IPluginFactory* p, pluginMap.values()) {
        foreach(QString depId, p->dependPluginList()) {
            idxMap.insert(depId, idxMap.value(depId) - 1);
        }
    }
    QMultiMap<int, IPluginFactory*> deps;
    QMapIterator<QString, int> i(idxMap);
    while (i.hasNext()) {
        i.next();
        deps.insert(i.value(), pluginMap.value(i.key()));
    }
    QList<int> keys = deps.keys();
    QSet<int>  keySets;
    QList<int> temp;
    for (int v : keys) {
        if (!keySets.contains(v)) {
            keySets.insert(v);
            temp.append(v);
        }
    }
    keys = temp;
    std::sort(keys.begin(), keys.end());
    foreach(int index, keys) {
        foreach(IPluginFactory * p, deps.values(index)) {
            plugin_list_.append(p);
        }
    }
}

QList<IPluginFactory*> PluginManager::getPluginList() const
{
    return plugin_list_;
}

