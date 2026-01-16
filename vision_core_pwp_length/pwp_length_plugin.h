#ifndef PWP_LENGTH_PLUGIN_H
#define PWP_LENGTH_PLUGIN_H

#include <QObject>
#include "ialgorithmplugin.h"

class PwpLengthAlgorithmPlugin : public QObject, public IAlgorithmPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID AlgorithmPlugin_IID)
        Q_INTERFACES(IAlgorithmPlugin)

public:
    // 修改这里：给你的新算法起个唯一的ID
    QString name() const override { return "pwp_length"; }
    // 修改这里：中文描述
    QString description() const override { return "PWP长度测量"; }
    // 修改这里：所属分类
    QString category() const override { return "尺寸测量"; }

    std::shared_ptr<IAlgorithm> createInstance() override;
    PageBase* createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent = nullptr) override;
};

#endif // PWP_LENGTH_PLUGIN_H