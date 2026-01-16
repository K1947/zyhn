#ifndef IALGORITHM_PLUGIN_H
#define IALGORITHM_PLUGIN_H

#include <QString>
#include <QtPlugin>
#include "vision_core_global.h"

class IAlgorithm;
class PageBase;
class AlgoPageManager;
class IAlgorithmPlugin
{
public:
    virtual ~IAlgorithmPlugin() = default;
    virtual QString name() const = 0;
    virtual QString category() const = 0;
    virtual QString description() const = 0;
    virtual std::shared_ptr<IAlgorithm> createInstance() = 0;
    virtual PageBase* createPageView(const QString& type, AlgoPageManager* mgr, QWidget* parent = nullptr) = 0;
};

#define AlgorithmPlugin_IID "com.vision.core.IAlgorithmPlugin"
Q_DECLARE_INTERFACE(IAlgorithmPlugin, AlgorithmPlugin_IID)

#endif // !IALGORITHM_PLUGIN_H