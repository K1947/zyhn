#include "circlefit_algorithm.h"
#include "property_editor_widget.h"
#include "configurable_object.h"
#include <Eigen/Dense>
#include <Eigen/QR>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ctime>
#include "drawing_result_manager.h"
#include "result_shape.h"
#include "algorithm_input.h"
#include "logger_utils.h"
using namespace std::chrono;


CircleFitAlgorithm::CircleFitAlgorithm(): 
    prop_obj_(new ConfigurableObject()),
    center_(Pointf(0.0f, 0.0f)),
    inner_radius_(0.0f),
    outer_radius_(0.0f),
    init_param_status_(false)
{
    setupProperties();
}

CircleFitAlgorithm::~CircleFitAlgorithm()
{

}

bool CircleFitAlgorithm::initialize()
{
    return true;
}

void CircleFitAlgorithm::setupProperties()
{
    PropertyBuilder::create("选择图像源", "image_source")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"图像1", ""} })
        .defaultValue("图像1")
        .registerTo(prop_obj_);

    // 新增阈值调整函数
    PropertyBuilder::create("设置拟合点数", "handleNum")
        .category("基本参数")
        .type(QMetaType::Int)
        .defaultValue(36)
        .registerTo(prop_obj_);

    PropertyBuilder::create("选择测量极性", "transition")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"从暗到明", 0},
                {"从明到暗", 1},
                {"所有", 2} })
        .defaultValue("从暗到明")
        .registerTo(prop_obj_);

    PropertyBuilder::create("选择测量方向", "direction")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"从内向外", 0},
                {"从外向内", 1} })
        .defaultValue("从内向外")
        .registerTo(prop_obj_);

    // 新增阈值调整函数
    PropertyBuilder::create("设置测量阈值", "threshold")
        .category("基本参数")
        .type(QMetaType::Int)
        .defaultValue(15)
        .registerTo(prop_obj_);

    PropertyBuilder::create("选择点模式", "select")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"第一点", 0},
                {"最后一点", 1},
                {"所有", 2} })
        .defaultValue("第一点")
        .registerTo(prop_obj_);

    PropertyBuilder::create("权重类型", "weight_type")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"tukey", 0},
                {"huber", 1} })
        .defaultValue("tukey")
        .registerTo(prop_obj_);

    PropertyBuilder::create("标准圆圆心x坐标", "std_cx")
        .category("管控参数")
        .type(QMetaType::Int)
        .defaultValue(150)
        .registerTo(prop_obj_);

    PropertyBuilder::create("标准圆圆心y坐标", "std_cy")
        .category("管控参数")
        .type(QMetaType::Int)
        .defaultValue(250)
        .registerTo(prop_obj_);

    PropertyBuilder::create("标准圆半径", "std_radius")
        .category("管控参数")
        .type(QMetaType::Int)
        .defaultValue(250)
        .registerTo(prop_obj_);

    PropertyBuilder::create("允许偏差值", "permissibleErr")
        .category("管控参数")
        .type(QMetaType::Double)
        .defaultValue(5.0)
        .registerTo(prop_obj_);

    prop_obj_->registerBtn(1, "保存模板图像");
    prop_obj_->registerBtn(2, "测试运行");
}

QString CircleFitAlgorithm::name() const
{
    return "circlefit";
}

ConfigurableObject* CircleFitAlgorithm::getPropertyObj()
{
    return prop_obj_;
}

void CircleFitAlgorithm::setParameters(const QJsonObject& p)
{
    prop_obj_->setPropetyFromJson(p);

    std::shared_ptr<RoiRing> ring = nullptr;
    for (auto roi : p["rois"].toArray())
    {
        auto shape = RoiShape::fromJson(roi.toObject());
        if (shape->type() == RoiShape::Ring) {
            ring = std::static_pointer_cast<RoiRing>(shape);
            break;
        }
    }

    if (!ring) {
        return;
    }

    center_ = Pointf(ring->center().x(), ring->center().y());
    inner_radius_ = ring->innerRadius();
    outer_radius_ = ring->outterRadius();
    init_param_status_ = true;
}

void CircleFitAlgorithm::setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p)
{
    for (auto it = p.begin(); it != p.end(); it++) {
        prop_obj_->setPropertyEnumValues(it.key(), it.value());
    }
}

// ========================== 私有方法实现 ===================== // 
    // 1. 鲁棒性的初始估计
void CircleFitAlgorithm::computeSmallDataInitialEstimate(const std::vector<Pointf>& points, double& center_x, double& center_y, double& radius)
{
    const int n = points.size();
    if (n < 3) {
        center_x = 0.0;
        center_y = 0.0;
        radius = 0.0;
        return;
    }
    // 方法1：使用中位数而不是平均值
    std::vector<double> x_vals, y_vals;
    for (const auto& p : points) {
        x_vals.push_back(p.x);
        y_vals.push_back(p.y);
    }
    std::sort(x_vals.begin(), x_vals.end());
    std::sort(y_vals.begin(), y_vals.end());

    center_x = x_vals[n / 2];
    center_y = y_vals[n / 2];

    // 计算基于中位数的半径
    std::vector<double> distances;
    for (const auto& p : points) {
        double dx = p.x - center_x;
        double dy = p.y - center_y;
        distances.push_back(std::sqrt(dx * dx + dy * dy));
    }
    std::sort(distances.begin(), distances.end());
    radius = distances[distances.size() / 2];
}

// 2. 计算几何距离残差 
double CircleFitAlgorithm::geometricResidual(double x, double y, double cx, double cy, double r)
{
    double dx = x - cx;
    double dy = y - cy;
    double dist = std::sqrt(dx * dx + dy * dy);
    return std::abs(dist - r);
}

// 3. Huber权重函数，效果不及Tukey
double CircleFitAlgorithm::HuberWeight(double residual, double k)
{
    double abs_r = std::abs(residual);
    return (abs_r <= k) ? 1.0 : k / abs_r;
}

// 4. Tukey权重函数
double CircleFitAlgorithm::TukeyWeight(double residual, double k)
{
    double r_over_k = residual / k;
    if (std::abs(r_over_k) <= 1.0) {
        double tmp = 1.0 - r_over_k * r_over_k;
        return tmp * tmp;
    }
    return 0.0;
}

// 5. 圆的一致性验证函数
double CircleFitAlgorithm::validateCircleConsistency(const std::vector<Pointf>& points, double center_x, double center_y, double radius)
{
    const double m_PI = 3.1415926;
    // 验证圆的几何一致性
    if (points.size() < 3) return 0.0;
    double maxAngularErr = 0.0;
    std::vector<double> angles;
    // 计算每个点相对于圆心的角度
    for (const auto& p : points) {
        double dx = p.x - center_x;
        double dy = p.y - center_y;
        double angle = std::atan2(dy, dx);
        angles.push_back(angle);
    }
    // 检查角度分布是否合理
    std::sort(angles.begin(), angles.end());
    // 计算最大角度间隙
    double max_gap = 0.0;
    for (size_t i = 0; i < angles.size(); i++) {
        double gap = (i == angles.size() - 1) ? (2 * m_PI + angles[0] - angles[i]) :
            (angles[i + 1] - angles[i]);
        max_gap = std::max(max_gap, gap);
    }
    return max_gap;
}

// =========================== 圆拟合测试函数 ======================== // 
Circle CircleFitAlgorithm::FitCircle(const std::vector<Pointf>& points, bool& IsOK, const std::string& method)
{
    FitConfig config;
    Circle result;
    if (method == "auto") {
        result = fitCircleIRLSOptimized(points, config);
    }
    else {
        result = fitCircleIRLSOptimized_plus(points, config, IsOK);
    }
    return result;
}

Circle CircleFitAlgorithm::FitCircle(const std::vector<Pointf>& points, const FitConfig& config)
{
    return fitCircleIRLSOptimized(points, config);
}

// ==================== 简单的最小二乘拟合算法 ======================== //
Circle CircleFitAlgorithm::fitCircleSimpleLS(const std::vector<Pointf>& points)
{
    // 简单的线性最小二乘圆拟合
    const int n = points.size();
    if (n < 3) {
        return Circle(0, 0, 0);
    }
    Eigen::MatrixXd A(n, 3);
    Eigen::VectorXd b(n);
    // 简单的最小二乘拟合求解
    for (int i = 0; i < n; i++) {
        double x = points[i].x;
        double y = points[i].y;
        A(i, 0) = x;
        A(i, 1) = y;
        A(i, 2) = 1.0;
        b(i) = x * x + y * y;
    }
    // 矩阵求解
    Eigen::Vector3d params = A.colPivHouseholderQr().solve(b);
    double center_x = params(0) / 2.0;
    double center_y = params(1) / 2.0;
    double radius_sq = params(2) + center_x * center_x + center_y * center_y;
    double radius = (radius_sq > 0) ? std::sqrt(radius_sq) : 0.0;
    Circle result(center_x, center_y, radius);
    result.error = CalculateRMSE(points, center_x, center_y, radius);
    result.method = "SimpleLS";
    result.IsConverged = true;
    return result;
}

// ======================= 圆拟合核心算法实现 ===================== //
Circle CircleFitAlgorithm::fitCircleIRLSOptimized(const std::vector<Pointf>& points, const FitConfig& config)
{
    auto start = system_clock::now();
    // 计算圆拟合点的数量
    const int n = points.size();
    // 选择权重函数
    auto weight_func = (prop_obj_->propValue("weight_type") == "tukey") ? TukeyWeight : HuberWeight;
    // 预计算点坐标和平方和
    Eigen::VectorXd x_vec(n), y_vec(n), z_vec(n);
    double center_x = 0.0, center_y = 0.0, radius = 0.0;
    for (int i = 0; i < n; i++) {
        x_vec(i) = points[i].x;
        y_vec(i) = points[i].y;
        z_vec(i) = points[i].x * points[i].x + points[i].y * points[i].y;
    }
    // 初始圆心估计（使用所有点的平均值）
    center_x = x_vec.mean();
    center_y = y_vec.mean();
    // 初始半径估计
    computeSmallDataInitialEstimate(points, center_x, center_y, radius);
    // 初始化权重全部设为1
    Eigen::VectorXd weights = Eigen::VectorXd::Ones(n);
    // 预分配内存
    Eigen::MatrixXd A(n, 3);
    Eigen::VectorXd b(n);
    Eigen::VectorXd residuals(n);
    // IRLS迭代
    int iter = 0;
    double adaptive_kFactor = config.k_factor;  // prop_obj_->propValue("factor").toDouble();
    bool converged = false;
    double prev_err = (std::numeric_limits<double>::max());
    int maxIterations = config.max_iterations;  // prop_obj_->propValue("iterations").toInt();
    for (iter = 0; iter < maxIterations; ++iter) {
        // 计算当前残差（几何距离）---- 所有候选点进行迭代
        for (int i = 0; i < n; ++i) {
            double dx = x_vec(i) - center_x;
            double dy = y_vec(i) - center_y;
            residuals(i) = std::abs(std::sqrt(dx * dx + dy * dy) - radius);
        }
        // 迭代一轮之后计算当前误差
        double cur_err = residuals.norm() / std::sqrt(n);
        // 误差计算完成后，计算是否收敛，可提前结束迭代
        if (std::abs(prev_err - cur_err) <= config.tolerance || cur_err < config.tolerance) {
            converged = true;
            break;
        }
        // 不满足要求，则更新当前的误差
        prev_err = cur_err;
        // 计算残差的中位数绝对偏差（MAD）来估计尺度参数
        Eigen::VectorXd sorted_residuals = residuals;
        std::sort(sorted_residuals.data(), sorted_residuals.data() + n);
        double q1 = sorted_residuals(n / 4);
        double q3 = sorted_residuals(3 * n / 4);
        double iqr = q3 - q1;
        // 自适应调整k_factor,在初期更容忍离群点
        if (iter < 3) {
            adaptive_kFactor = 3.0 * config.k_factor;		// 初期宽松
        }
        else if (iter < 8) {
            adaptive_kFactor = 2.0 * config.k_factor;		// 中期适中
        }
        else {
            adaptive_kFactor = config.k_factor;				// 后期严格
        }
        // 估计偏差值
        double sigma = (iqr > 1e-6) ? iqr / 1.349 : 1.0;
        double k = adaptive_kFactor * sigma;
        // 更新权重 ----- 优化小数据集的适配性
        for (int i = 0; i < n; ++i) {
            // weights(i) = weight_func(residuals(i), k);
            // 小数据集避免完全排除任何点
            if (n < 15 && residuals(i) > 4.0 * k) {
                weights(i) = 0.1;		// 保留最小权重而不是完全排除
            }
            else {
                weights(i) = weight_func(residuals(i), k);
            }
        }
        // 构建加权最小二乘问题
        for (int i = 0; i < n; ++i) {
            // 防止权重为0导致数值问题
            double w = std::sqrt(std::max(weights(i), 1e-12));
            A(i, 0) = w * x_vec(i);
            A(i, 1) = w * y_vec(i);
            A(i, 2) = w;
            b(i) = w * z_vec(i);
        }
        // 使用QR分解求解加权最小二乘问题
        Eigen::Vector3d params;
        try {
            Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(A);
            if (qr.rank() < 3) {
                // 秩不足，使用SVD
                params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
            }
            else {
                params = qr.solve(b);
            }
        }
        catch (const std::exception& e) {
            params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
        }
        // 从分解的参数中获取圆心和半径
        double new_center_x = params(0) / 2.0;
        double new_center_y = params(1) / 2.0;
        double new_radius_sq = (params(2) + new_center_x * new_center_x + new_center_y * new_center_y);
        // 确保半径平方为正数
        double new_radius = std::sqrt(new_radius_sq);
        // 检查新解是否合理
        if (new_radius > 1e-6 && std::isfinite(new_radius)) {
            // 优化10：添加阻尼更新，避免剧烈变化
            double damping = 0.15;		// 阻尼因子
            center_x = damping * center_x + (1 - damping) * new_center_x;
            center_y = damping * center_y + (1 - damping) * new_center_y;
            radius = damping * radius + (1 - damping) * new_radius;
        }
    }
    Circle result(center_x, center_y, radius);
    // 单次迭代完成之后，进行计算最终拟合误差
    /*result.error = CalculateRMSE(points, center_x, center_y, radius);
    result.iterations = iter;
    result.method = "IRLS_(" + config.weight_type + ")";
    result.IsConverged = converged;
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
    std::cout << "标准算法所耗费时间为:" << diff << "ms" << std::endl;*/
    return result;
}

// 6. =============== 再优化一般小的圆拟合算法实现 ======================//
Circle CircleFitAlgorithm::fitCircleIRLSOptimized_plus(const std::vector<Pointf>& points, const FitConfig& config, bool& IsOK)
{
    // 拟合之前默认结果是正确的
    IsOK = true;
    auto start = system_clock::now();
    // 计算圆拟合点的数量
    const int n = points.size();
    // 选择权重函数
    auto weight_func = (prop_obj_->propValue("weight_type") == "tukey") ? TukeyWeight : HuberWeight;

    // 预计算点坐标和平方和
    Eigen::VectorXd x_vec(n), y_vec(n), z_vec(n);
    double center_x = 0.0, center_y = 0.0, radius = 0.0;
    for (int i = 0; i < n; i++) {
        x_vec(i) = points[i].x;
        y_vec(i) = points[i].y;
        z_vec(i) = points[i].x * points[i].x + points[i].y * points[i].y;
    }
    // 初始圆心估计（使用所有点的平均值）
    center_x = x_vec.mean();
    center_y = y_vec.mean();
    // 初始半径估计
    computeSmallDataInitialEstimate(points, center_x, center_y, radius);
    // 初始化权重全部设为1
    Eigen::VectorXd weights = Eigen::VectorXd::Ones(n);
    // 预分配内存
    Eigen::MatrixXd A(n, 3);
    Eigen::VectorXd b(n);
    Eigen::VectorXd residuals(n);
    // IRLS迭代
    int iter = 0;
    double adaptive_kFactor = config.k_factor;  //prop_obj_->propValue("factor").toDouble();
    bool converged = false;
    double prev_err = std::numeric_limits<double>::max();
    int maxIterations = config.max_iterations;  // prop_obj_->propValue("iterations").toInt();
    for (iter = 0; iter < maxIterations; ++iter) {
        // 计算当前残差（几何距离）---- 所有候选点进行迭代
        for (int i = 0; i < n; ++i) {
            double dx = x_vec(i) - center_x;
            double dy = y_vec(i) - center_y;
            residuals(i) = std::abs(std::sqrt(dx * dx + dy * dy) - radius);
        }
        // 迭代一轮之后计算当前误差
        double cur_err = residuals.norm() / std::sqrt(n);
        // 误差计算完成后，计算是否收敛，可提前结束迭代
        if (std::abs(prev_err - cur_err) <= config.tolerance || cur_err < config.tolerance) {
            converged = true;
            break;
        }
        // 不满足要求，则更新当前的误差
        prev_err = cur_err;
        // 计算残差的中位数绝对偏差（MAD）来估计尺度参数
        Eigen::VectorXd sorted_residuals = residuals;
        std::sort(sorted_residuals.data(), sorted_residuals.data() + n);
        double q1 = sorted_residuals(n / 4);
        double q3 = sorted_residuals(3 * n / 4);
        double iqr = q3 - q1;
        // 自适应调整k_factor,在初期更容忍离群点
        if (iter < 3) {
            adaptive_kFactor = 3.0 * config.k_factor; // prop_obj_->propValue("factor").toDouble();		// 初期宽松 3， 3.0
        }
        else if (iter < 8) {
            adaptive_kFactor = 2.0 * config.k_factor; // prop_obj_->propValue("factor").toDouble();		// 中期适中 8， 2.0
        }
        else {
            adaptive_kFactor = config.k_factor;       // prop_obj_->propValue("factor").toDouble();	    // 后期严格
        }
        // 估计偏差值
        double sigma = (iqr > 1e-6) ? iqr / 1.349 : 1.0;
        double k = adaptive_kFactor * sigma;
        // 更新权重----适应小数据集
        for (int i = 0; i < n; ++i) {
            // 小数据集避免完全排除任何点
            if (n < 15 && residuals(i) > 4.0 * k) {
                weights(i) = 0.1;		// 保留最小权重而不是完全排除
            }
            else {
                weights(i) = weight_func(residuals(i), k);
            }
        }
        // 构建加权最小二乘问题
        for (int i = 0; i < n; ++i) {
            // 防止权重为0导致数值问题
            double w = std::sqrt(std::max(weights(i), 1e-12));
            A(i, 0) = w * x_vec(i);
            A(i, 1) = w * y_vec(i);
            A(i, 2) = w;
            b(i) = w * z_vec(i);
        }
        // 使用QR分解求解加权最小二乘问题
        Eigen::Vector3d params;
        try {
            Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr(A);
            if (qr.rank() < 3) {
                // 秩不足，使用SVD
                params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
            }
            else {
                params = qr.solve(b);
            }
        }
        catch (const std::exception& e) {
            params = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
        }
        // 从分解的参数中获取圆心和半径
        double new_center_x = params(0) / 2.0;
        double new_center_y = params(1) / 2.0;
        double new_radius_sq = (params(2) + new_center_x * new_center_x + new_center_y * new_center_y);
        // 确保半径平方为正数
        double new_radius = std::sqrt(new_radius_sq);
        // 检查新解是否合理
        if (new_radius > 1e-6 && std::isfinite(new_radius)) {
            // 优化10：添加阻尼更新，避免剧烈变化
            double damping = 0.15;		// 阻尼因子
            center_x = damping * center_x + (1 - damping) * new_center_x;
            center_y = damping * center_y + (1 - damping) * new_center_y;
            radius = damping * radius + (1 - damping) * new_radius;
        }
    }
    Circle result(center_x, center_y, radius);
    // 新增判断圆拟合结果是否正常
    float cxErr = 0, cyErr = 0, radiusErr = 0, permissibleErr = 0;
    cxErr = fabs(center_x - prop_obj_->propValue("std_cx").toInt());
    cyErr = fabs(center_y - prop_obj_->propValue("std_cy").toInt());
    radiusErr = fabs(radius - prop_obj_->propValue("std_radius").toInt());
    permissibleErr = prop_obj_->propValue("permissibleErr").toDouble();
    if (cxErr > permissibleErr || cyErr > permissibleErr || radiusErr > permissibleErr) {
        IsOK = false;
    }
    // 单次迭代完成之后，进行计算最终拟合误差
    /*result.error = CalculateRMSE(points, center_x, center_y, radius);
    result.iterations = iter;
    result.method = "IRLS_(" + prop_obj_->propValue("weight_type").toString().toStdString() + ")";
    result.IsConverged = converged;
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - start).count();
    std::cout << "标准算法所耗费时间为:" << diff << "ms" << std::endl;*/
    return result;
}

double CircleFitAlgorithm::CalculateRMSE(const std::vector<Pointf>& points, double center_x, double center_y, double radius)
{
    double error = 0.0;
    for (const auto& p : points) {
        double dx = p.x - center_x;
        double dy = p.y - center_y;
        double dist = std::sqrt(dx * dx + dy * dy);
        error += (dist - radius) * (dist - radius);
    }
    return std::sqrt(error / points.size());
}

// ======================= 新增卡尺测量模块 =============================== //
int CircleFitAlgorithm::measureCirclePts(HalconCpp::HObject& hSrc, Pointf& center, const float& innerRadius, const float& outterRadius, std::vector<Pointf>* allPts)
{
    allPts->clear();
    using namespace HalconCpp;
    HObject nullImg;
    HTuple hRes, width, height;
    GenEmptyObj(&nullImg);
    TestEqualObj(hSrc, nullImg, &hRes);
    if (hRes == 1) {
        zyhn_core::LoggerUtils::error("circlefit","输入图像为空!");
        return -1;
    }
    GetImageSize(hSrc, &width, &height);
    // 输入的圆环中心，内外径大小
    double cCol = 0.0, cRow = 0.0;
    cRow = center.y;
    cCol = center.x;
    float measureLength = (outterRadius - innerRadius);        
    if (measureLength < 5) {
        return -1;                                                    // 测量长度错误
    }
    // 确保测量长度不低于5个像素长度 (调试阶段使用断言，发布阶段关闭断言提高程序运行效率)
    /* 如果condition判断结果为false，则程序就会停止执行；否则继续执行*/
    float measureRadius = 0.5 * (outterRadius + innerRadius);
    // 计算测量的角度步长范围
    int m_handleNum = prop_obj_->propValue("handleNum").toInt();
    float stepAngle = 360.0 / m_handleNum;		// 36为需要测量的卡尺点数量
    std::string transition = "";
    QString transitionProp = prop_obj_->propValue("transition").toString();
    // step1: 选择测量极性
    if (transitionProp == "从暗到明") {
        transition = "positive";
    }
    else if (transitionProp == "从明到暗") {
        transition = "negative";
    }
    else {
        transition = "all";
    }
    // step2: 选择测量输出点
    std::string select = "";
    QString selectProp = prop_obj_->propValue("select").toString();
    if (selectProp == "第一点") {
        select = "first";
    }
    else if (selectProp == "最后一点") {
        select = "last";
    }
    else {
        select = "all";
    }
    // ====================== 遍历测量点 ========================== //
    QString directionProp = prop_obj_->propValue("direction").toString();
    for (int i = 0; i < m_handleNum; ++i) {
        float curAngle = i * stepAngle;
        if (curAngle < 0 || curAngle >= 360) {
            continue;
        }
        float R = 0.0, C = 0.0, L1 = 0.0, L2 = 0.0;
        L1 = measureLength / 2.0;
        L2 = 5.0;
        float measurePhi = curAngle * PI / 180.0;
        // 按照四个象限划分不同的区域,计算测量矩形的中心
        if (curAngle > 0 && curAngle <= 90) {
            R = cRow - fabs(measureRadius * sin(measurePhi));
            C = cCol + fabs(measureRadius * cos(measurePhi));
        }
        else if (curAngle > 90 && curAngle <= 180) {
            R = cRow - fabs(measureRadius * sin(measurePhi));
            C = cCol - fabs(measureRadius * cos(measurePhi));
        }
        else if (curAngle > 180 && curAngle <= 270) {
            R = cRow + fabs(measureRadius * sin(measurePhi));
            C = cCol - fabs(measureRadius * cos(measurePhi));
        }
        else {
            R = cRow + fabs(measureRadius * sin(measurePhi));
            C = cCol + fabs(measureRadius * cos(measurePhi));
        }
        // 测量方向改变后需调整角度大小
        if (directionProp == "由内向外") {
            if (measurePhi > PI)
                measurePhi -= 2 * PI;
        }
        else {
            measurePhi -= PI;
        }
        // step3: 调用halcon的卡尺测量模块进行卡尺测量
        float m_sigma = 1.0;
        int m_threshold = prop_obj_->propValue("threshold").toInt();
        HTuple rowEdge, colEdge, amplitude, distance, measureHandle;
        // try {
        GenMeasureRectangle2(R, C, measurePhi, L1, L2, width, height, "bilinear", &measureHandle);
        MeasurePos(hSrc, measureHandle, m_sigma, m_threshold, transition.c_str(), select.c_str(),
            &rowEdge, &colEdge, &amplitude, &distance);
        CloseMeasure(measureHandle);
        /*catch (HException& except) {
            std::cout << "是否存在错误编码:" << except.ErrorCode();
            return except.ErrorCode();
        }*/
        // 测量一次则储存一次测量出来的候选拟合点
        // std::cout << "测量点是否正常:" << rowEdge.Length() << "数值:" << rowEdge.D() << colEdge.D() << std::endl;
        if (rowEdge.Length() > 0) {
            // std::cout << "测量点是否正常:" << rowEdge.Length() << "数值:" << rowEdge.D() << colEdge.D() << std::endl;
            for (int i = 0; i < rowEdge.Length(); i++) {
                allPts->push_back(Pointf(colEdge[i], rowEdge[i]));
                // std::cout << "已经正确储存" << std::endl;
            }
        }
    }
    return 0;
}

AlgorithmResult CircleFitAlgorithm::test(const AlgorithmInput& input)
{
    AlgorithmResult result;
    uint8_t err = 0;
    int idx = prop_obj_->enumIndex("image_source");
    if (idx >= input.imageCount()) {
        result.setCode(-1);
        result.setMsg("图像索引超出范围");
        return result;
    }

    hImg = input.getImageAsHObject(idx);
    /*HalconCpp::HTuple channels;
    CountChannels(hImg, &channels);
    int chCount = channels.I();
    qDebug() << "[HairyFabric] Detected Channels:" << chCount;*/
    if (!hImg.IsInitialized()) {
        result.setCode(-2);
        result.setMsg("获取图片失败");
        return result;
    }

    QVector<std::shared_ptr<RoiShape>> roishpes = input.roiShapes();
    auto it = std::find_if(roishpes.begin(), roishpes.end(),
        [](const std::shared_ptr<RoiShape>& shapePtr) {
            return shapePtr->type() == RoiShape::Ring;
    });

    std::shared_ptr<RoiRing> ring = nullptr;
    if (it != roishpes.end()) {
        ring = std::static_pointer_cast<RoiRing>(*it);
    }

    if (!ring) {
        result.setCode(-3);
        result.setMsg("请先绘制ring roi");
        return result;
    }

    std::vector<Pointf> allPts;
    center_.x = ring->center().x();
    center_.y = ring->center().y();

    inner_radius_ = ring->innerRadius();
    outer_radius_ = ring->outterRadius();

    err = measureCirclePts(hImg, center_, inner_radius_, outer_radius_, &allPts);
    if (err != 0) {
        result.setCode(err);
        result.setMsg("卡尺测量算法出错");
        return result;
    }

    for (auto pt : allPts) {
        auto rect = std::make_shared<ResultRect>(QRectF(pt.x, pt.y, 4, 4), Qt::red);
        result.addResultShape(rect);
    }
    // 测量圆的大小：
    bool cirStatus;
    Circle result2 = FitCircle(allPts, cirStatus, "plus");

    auto resultCircle = std::make_shared<ResultCircle>(QPointF(result2.center_x, result2.center_y), result2.radius);
    result.addResultShape(resultCircle);

    // 增加一个十字标记
    auto coordinate = std::make_shared<ResultCoordinate>(QPointF(result2.center_x, result2.center_y), 0);
    result.addResultShape(coordinate);

    QString content = QString("center: x=%1， y=%2").arg(result2.center_x).arg(result2.center_y);
    auto textItem = std::make_shared<ResultText>(content, QPointF(0, 0), 48);
    result.addResultShape(textItem);

    return result;
}

AlgorithmResult CircleFitAlgorithm::run(const AlgorithmContext& context)
{
    AlgorithmResult result;
    if (context.input.imageCount() <= 0) {
        result.setCode(-1);
        result.setMsg("图像不存在");
        return result;
    }

    hImg = context.input.getImageAsHObject(0);
    if (!hImg.IsInitialized()) {
        result.setCode(-2);
        result.setMsg("获取图片失败");
        return result;
    }

    if (!init_param_status_) {
        result.setCode(-3);
        result.setMsg("初始化参数失败");
        return result;
    }

    std::vector<Pointf> allPts;
    int err = 0;
    err = measureCirclePts(hImg, center_, inner_radius_, outer_radius_, &allPts);
    if (err != 0) {
        result.setCode(err);
        result.setMsg("圆拟合卡尺测量算法出错");
        return result;
    }    

    for (auto pt : allPts) {
        auto rect = std::make_shared<ResultRect>(QRectF(pt.x, pt.y, 4, 4), Qt::red);
        result.addResultShape(rect);
    }
    // 测量圆的大小：
    bool cirStatus;
    Circle result2 = FitCircle(allPts, cirStatus, "plus");

    auto resultCircle = std::make_shared<ResultCircle>(QPointF(result2.center_x, result2.center_y), result2.radius);
    result.addResultShape(resultCircle);

    // 增加一个十字标记
    auto coordinate = std::make_shared<ResultCoordinate>(QPointF(result2.center_x, result2.center_y), 0);
    result.addResultShape(coordinate);

    QString content = QString("center: x=%1， y=%2, r=%3").arg(result2.center_x).arg(result2.center_y).arg(result2.radius);
    auto textItem = std::make_shared<ResultText>(content, QPointF(0, 0), 32);
    result.addResultShape(textItem);

    // 新增圆拟合管控----超过设置的值则报失败
    if (cirStatus) {
        result.setResultType(ResultType::OK);
    } else {
        result.setResultType(ResultType::NG);
    }
    return result;
}