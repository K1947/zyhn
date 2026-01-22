#ifndef PWP_LENGTH_ALGORITHM_H
#define PWP_LENGTH_ALGORITHM_H

#include "ialgorithm.h"
#include "configurable_object.h"
#include <QVector>
#include <memory>

// 引入 Halcon 头文件
#define NOMINMAX
#include <halconCpp/HalconCpp.h>

// 前置声明
class RoiShape;

class PwpLengthAlgorithm : public IAlgorithm
{
public:
    PwpLengthAlgorithm();
    ~PwpLengthAlgorithm();

    virtual bool initialize() override;
    virtual QString name() const override;

    // 参数设置相关接口
    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p);
    virtual ConfigurableObject* getPropertyObj() override;
    virtual void setParameters(const QJsonObject& p) override;

    // 运行接口
    virtual AlgorithmResult run(const AlgorithmContext& input) override;
    AlgorithmResult test(const AlgorithmInput& input);

private:
    void setupProperties();
    // 辅助函数：将 ROI 形状列表转换为 Halcon 区域
    HalconCpp::HObject getRoiRegion(const QVector<std::shared_ptr<RoiShape>>& shapes);

private:
    ConfigurableObject* prop_obj_;
    bool init_param_status_;
};

#endif // PWP_LENGTH_ALGORITHM_H