#ifndef DEFINE_H
#define DEFINE_H
#include <string>


// 二维点结构体定义
struct Pointf {
    double x;
    double y;
    Pointf(double x_ = 0.0, double y_ = 0.0) : x(x_), y(y_) {}
};

// 圆拟合结果结构体
struct Circle {
    double center_x;
    double center_y;
    double radius;
    double error;		// 圆拟合误差
    int iterations;		// 迭代次数
    std::string method; // 使用的权重函数（Tukey 或 huber）
    bool IsConverged;	// 增加判断是否收敛
    Circle() = default;	// 新增一个默认构造函数
    Circle(double cx, double cy, double r, double err = 0.0, int iter = 0,
        const std::string& m = "", bool conv = true) :
        center_x(cx), center_y(cy), radius(r), error(err),
        iterations(iter), method(m), IsConverged(conv) {
    }
};

// 拟合配置参数
struct FitConfig {
    std::string weight_type = "tukey";  // tukey（对离散点更好）或huber(对小半径较好)
    int max_iterations = 50;
    double tolerance = 1e-3;
    double k_factor = 1.345;

    FitConfig() = default;
    FitConfig(const std::string& w, int iter, double tol, double k) :
        weight_type(w), max_iterations(iter), tolerance(tol), k_factor(k) {
    }
    // 显示声明默认析构函数
    ~FitConfig() = default;
};

#endif // !DEFINE_H
