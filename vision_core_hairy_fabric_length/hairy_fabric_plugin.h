#ifndef HAIRY_FABRIC_PLUGIN_H
#define HAIRY_FABRIC_PLUGIN_H

#include <QObject>
#include "ialgorithmplugin.h"

class HairyFabricPlugin : public QObject, public IAlgorithmPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID AlgorithmPlugin_IID)
        Q_INTERFACES(IAlgorithmPlugin)

public:
    // 唯一标识符
    QString name() const override { return "hairy_fabric"; }
    // 中文名称
    QString description() const override { return "刺毛布检测"; }
    // 分类
    QString category() const override { return "尺寸测量"; }

    std::shared_ptr<IAlgorithm> createInstance() override;
    PageBase* createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent = nullptr) override;
};

#endif // HAIRY_FABRIC_PLUGIN_H