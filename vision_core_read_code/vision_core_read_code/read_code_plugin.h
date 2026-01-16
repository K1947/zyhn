#ifndef READ_CODE_PLUGIN_H
#define READ_CODE_PLUGIN_H

#include <QObject>
#include "ialgorithmplugin.h"

class ReadCodeAlgorithmPlugin : public QObject, public IAlgorithmPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID AlgorithmPlugin_IID)
        Q_INTERFACES(IAlgorithmPlugin)

public:
    QString name() const override { return "readcode"; }
    QString description() const override { return "扫码"; }
    QString category() const override { return "分类计数"; }

    IAlgorithm* createInstance() override;
    PageBase* createPageView(const QString& type, AlgoPageManager* mgr, QWidget* parent = nullptr) override;
    void destroyInstance(IAlgorithm* instance) override;
};

#endif // !READ_CODE_PLUGIN_H

