#ifndef IALGORITHM_H
#define IALGORITHM_H

#include <QString>
#include <QWidget>
#include <QJsonObject>
#include <QImage>
#include "vision_core_global.h"
#include "algorithm_context.h"

class ConfigurableObject;
class VISION_CORE_EXPORT IAlgorithm
{
public:
    virtual ~IAlgorithm() = default;
    virtual bool initialize() = 0;
    virtual QString name() const = 0;
    virtual void setParameters(const QJsonObject& obj) = 0;
    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p) {}
    virtual AlgorithmResult run(const AlgorithmContext& input) = 0;  // 自动运行的时候调用
    virtual ConfigurableObject* getPropertyObj() = 0;
};

#define IAlgorithm_iid "com.zyhn.Vision.IAlgorithm"
Q_DECLARE_INTERFACE(IAlgorithm, IAlgorithm_iid)

#endif // IALGORITHM_H