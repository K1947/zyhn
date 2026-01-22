#ifndef PWP_LENGTH_PLUGIN_H
#define PWP_LENGTH_PLUGIN_H

#include <QObject>
#include "ialgorithmplugin.h"

class PwpLengthPlugin : public QObject, public IAlgorithmPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID AlgorithmPlugin_IID)
        Q_INTERFACES(IAlgorithmPlugin)

public:
    QString name() const override { return "pwp_length"; }
    QString description() const override { return "PWP曲线测长"; }
    QString category() const override { return "尺寸测量"; }

    std::shared_ptr<IAlgorithm> createInstance() override;
    PageBase* createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent = nullptr) override;
};

#endif // PWP_LENGTH_PLUGIN_H