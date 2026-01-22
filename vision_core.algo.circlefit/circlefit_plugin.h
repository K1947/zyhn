#ifndef CIRCLEFIT_PLUGIN_H
#define CIRCLEFIT_PLUGIN_H

#include <QObject>
#include "ialgorithmplugin.h"

/*
圆拟合算法插件
*/
class CircleFitAlgorithmPlugin : public QObject, public IAlgorithmPlugin
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AlgorithmPlugin_IID)
    Q_INTERFACES(IAlgorithmPlugin)

public:
    // 算法名称，唯一标识符
    QString name() const override { return "circlefit"; }
    // 算法说明：解释该算法作用
    QString description() const override { return "圆拟合"; }
    // 该算法模块所属算法大类
    QString category() const override { return "尺寸测量"; }
    // 创建算法实例
    std::shared_ptr<IAlgorithm> createInstance() override;
    // 创建页面显示
    PageBase* createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent = nullptr) override;
};

#endif // CIRCLEFIT_PLUGIN_H