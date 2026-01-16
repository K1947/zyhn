#ifndef ALGORITHM_MANAGER_H
#define ALGORITHM_MANAGER_H

#include <QString>
#include <QMap>
#include <QDir>
#include <QPluginLoader>
#include <QSharedPointer>
#include <QStringList>
#include "vision_core_global.h"
#include "IAlgorithm.h"
#include "ialgorithmplugin.h"

class PageBase;
class AlgoPageManager;

class VISION_CORE_EXPORT AlgorithmManager
{
public:
    static AlgorithmManager& instance();
    void loadPlugins(const QString& folder);
    std::shared_ptr<IAlgorithm> createAlgorithm(const QString& name);
    PageBase* createAlgoPageView(const QString& name, AlgoPageManager* manager, QWidget* parent);
    QStringList availableAlgorithms() const;
    QMap<QString, QVariantMap> getAlgoConfigInfo() const;
    bool containsAlgo(const QString& name) const;

private:
    AlgorithmManager();
    ~AlgorithmManager() = default;
    AlgorithmManager(const AlgorithmManager&) = delete;
    AlgorithmManager& operator=(const AlgorithmManager&) = delete;

    IAlgorithmPlugin* getAlgoPlugin(const QString& name);
    QMap<QString, QString> pluginPathByName_;
    QMap<QString, QSharedPointer<QPluginLoader>> pluginLoaders_;

    QMap<QString, QVariantMap> algoConfigMap_;
};

#endif