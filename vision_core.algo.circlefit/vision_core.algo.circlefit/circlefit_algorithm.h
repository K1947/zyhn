#ifndef CIRCLE_FIT_ALGO_RITHM_H
#define CIRCLE_FIT_ALGO_RITHM_H
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QPoint>
#include <QObject>

#define NOMINMAX
#include <halconCpp/HalconCpp.h>

#include "define.h"
#include "ialgorithm.h"
#include "configurable_object.h"
#include "property_editor_widget.h"
#include "image_editor.h"


/*
    圆拟合算法功能类 ---- 所有算法模块均从IAlgorithm基类
*/
class CircleFitAlgorithm: public IAlgorithm
{
  public:
    // 圆拟合算法构造函数
    CircleFitAlgorithm();
    // 圆拟合算法析构函数
    ~CircleFitAlgorithm();
    // 虚函数----初始化功能
    virtual bool initialize() override;
    // 虚函数----算法名称
    virtual QString name() const override;
    // 虚函数----设置运行参数
    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p);
    // 虚函数----回去属性表对象
    virtual ConfigurableObject* getPropertyObj() override;
    // 虚函数----设置参数
    virtual void setParameters(const QJsonObject& p) override;
    // 虚函数----流程运行函数
    virtual AlgorithmResult run(const AlgorithmContext& input) override;
    // 参数配置界面
    AlgorithmResult test(const AlgorithmInput& input);

private:
    void setupProperties();
    Circle FitCircle(const std::vector<Pointf>& points, bool& IsOK, const std::string& method = "auto");

    // 基础圆拟合 （带参数设置）
    Circle FitCircle(const std::vector<Pointf>& points, const FitConfig& config);
    // 工具函数
    double CalculateRMSE(const std::vector<Pointf>& points, double center_x, double center_y, double radius);
    // 卡尺测量用于圆拟合的函数
    int measureCirclePts(HalconCpp::HObject& hSrc, Pointf& center, const float& innerRadius, const float& outterRadius, std::vector<Pointf>* pts);
    // 不需要声明构造函数，编译器会自动生成
    Circle fitCircleSimpleLS(const std::vector<Pointf>& points);
    // 私有方法
    Circle fitCircleIRLSOptimized(const std::vector<Pointf>& points, const FitConfig& config);
    // 升级优化版本----用于后续升级
    Circle fitCircleIRLSOptimized_plus(const std::vector<Pointf>& points, const FitConfig& config, bool& IsOK);

    double geometricResidual(double x, double y, double cx, double cy, double r);

    static double HuberWeight(double residual, double k);

    static double TukeyWeight(double residual, double k);

    // 新增测试函数--更鲁棒的初始估计
    void computeSmallDataInitialEstimate(const std::vector<Pointf>& points, double& center_x, double& center_y, double& radius);

    // 验证圆的一致性函数
    double validateCircleConsistency(const std::vector<Pointf>& points, double center_x, double center_y, double radius);

  private:
    ConfigurableObject* prop_obj_;
    HalconCpp::HObject hImg;
    Pointf  center_;
    double inner_radius_;
    double outer_radius_;
    bool  init_param_status_;
};

#endif // CIRCLE_FIT_ALGO_RITHM_H