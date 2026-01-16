#ifndef PWP_LENGTH_ALGORITHM_H
#define PWP_LENGTH_ALGORITHM_H

#include "ialgorithm.h"
#include "configurable_object.h"

// 引入必要的 Halcon 头文件
#define NOMINMAX
#include <halconCpp/HalconCpp.h>

class PwpLengthAlgorithm : public IAlgorithm
{
public:
    PwpLengthAlgorithm();
    ~PwpLengthAlgorithm();

    virtual bool initialize() override;
    virtual QString name() const override;
    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p);
    virtual ConfigurableObject* getPropertyObj() override;
    virtual void setParameters(const QJsonObject& p) override;

    // 核心运行函数
    virtual AlgorithmResult run(const AlgorithmContext& input) override;
    // 测试运行函数
    AlgorithmResult test(const AlgorithmInput& input);

private:
    void setupProperties();

private:
    ConfigurableObject* prop_obj_;
    bool init_param_status_;
};

#endif // PWP_LENGTH_ALGORITHM_H