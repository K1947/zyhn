#ifndef ALGORITHM_UTILS_H
#define ALGORITHM_UTILS_H

#include "algorithm_global.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QObject>
#include <QImage>

#ifdef HALCON_UTILS
#include <halconcpp/HalconCpp.h>
using namespace HalconCpp;
#endif // HALCON_UTILS

namespace zyhn_core {
    class CResult;
}

//模板匹配结果
struct TemplateResult {
    double baseRow = 0;
    double baseCol = 0;
    float originRowSv = 0;
    float originColSv = 0;
    float phiSv = 0;

    bool isFeature = false;
    double featureBaseRow = 0;
    double featureBaseCol = 0;
    float featureOriginRowSv = 0;
    float featureOriginColSv = 0;
    float featurePhiSv = 0;
};

//仿射变换参数
struct affineBaseParam {
    double row = 0.0;
    double col = 0.0;
    double phi = 0.0;
    double baseRow = 0.0;
    double baseCol = 0.0;
    float scale = 1.0;
    bool detectModel = true;
    bool isTabRoi = false;
    int tabIndex = 1;

    double offsetRow = 0.0;
    double offsetCol = 0.0;
    double offsetPhi = 0.0;
    double offsetBaseRow = 0.0;
    double offsetBaseCol = 0.0;
    double offsetBasePhi = 0.0;
};

//全局通用：MTF计算所需参数
struct markParam {
    double daokouW = 0.0;
    double daokouH = 0.0;
    double freqX = 0.5;
    double freqX2 = 0.25;
    double highThd = 128;
    double open = 10;
    double gamma = 1.0;
    int markType = 0;       //标定板类型 0-背景黑色mark白色；1-背景白色Mark黑色
};

// 视觉AOI结构参数----保存视觉日志使用
struct aoiSetPams {
    QString modelName;                  // 模型名称
    QString defectName;                 // 缺陷名称
    QString preWay;                     // 预处理方式
    int minThd;                         // 最小阈值
    int maxThd;                         // 最大阈值
};

/*!
 * \brief The IAlgorithm class
 * 所有算法函数开始必须检测形参是否可用，是否在范围以内
 * 所有算法函数必须以int返回，0 表示OK，其余为NG，宏定义在error.h文件
 * 函数命名需按照驼峰命名法
 */
class ALGORITHM_EXPORT AlgorithmUtils : public QObject
{
    Q_OBJECT
public:
    AlgorithmUtils();
    /**
     * @brief mat类型图片转Qimage
     * @param mat     [in]    输入mat图像
     * @param image   [out]    输出Qimage图像
     * @return 返回int  0：函数正常，1：参数异常
     */
    static int matToQImage(const cv::Mat mat, QImage& Image);
    /**
     * @brief Qimage类型图片转Mat
     * @param src     [in]    输入QImage图像
     * @param mimage   [out]    输出Mat图像
     * @return 返回int  0：函数正常，1：参数异常
     */
    static int qImageToMat(const QImage src, cv::Mat& mImage);

    static cv::Mat qImage2Mat(QImage src);

    static QImage mat2QImage(cv::Mat mat);   

    static int thresholdTest(cv::Mat src, cv::Mat& dst, int low, int high);

    static int binMat2QImage(cv::Mat& bin, QImage& src, QColor color);

    static int contours2QImage(std::vector<cv::Point2f>& pts, QImage& src, QColor color);

#ifdef HALCON_UTILS
    static QImage hobjectToQimage(const HalconCpp::HObject hobject);
    static HalconCpp::HObject qimageToHobject(const QImage src);
    // cv::Mat转换为Hobject
    static int matToHobject(const cv::Mat image, HObject& himage);
    // Hobject转换为cv::Mat
    static int HobjectToMat(HObject H_img, cv::Mat& mImg);
#endif // HALCON_UTILS

    /**
     * @brief yuv2rgb       uyvy转换成RGB
     * @param uyvy      输入uyvy指针，指针大小为width*height*2
     * @param rgb       输出rgb指针
     * @param width
     * @param height
     * @return
     */
    static zyhn_core::CResult yuv2rgb(unsigned char* uyvy, unsigned char* rgb, int width, int height);
    /**
     * @brief 获取二值图区域的最小外接正矩形
     * @param src     [in]    输入二值图
     * @return cv::Rect 最小正矩形
     */
    static cv::Rect getMatRoiRect(cv::Mat& src);
    /**
     * @brief 把在一起的点分组
     * @param points     [in]    输入的点集
     * @param dsts       [out]    输出分组后的点集
     * @param minDistance   [in]    判定同组的距离标准
     * @param minNum       [in]    必须大于1
     * @return 函数执行成功与否
     *         0：成功
     *         200：参数错误
     */
    static int dbscan(std::vector<cv::Point>& points, std::vector<std::vector<cv::Point>>& dsts, float minDistance, int minNum);
    /**
     * @brief    动态阈值算法
     * @param src     [in]    输入的原图
     * @param dst       [out]    输出的结果图
     * @param kernel   [in]    均值滤波的核大小
     * @param offset       [in]    俩图相减后的偏移量
     * @param isLight       [in]    取明还是取暗缺陷
     * @return 函数执行成功与否
     *         0：成功
     *         200：参数错误
     */
    static int dynThresholdTest(cv::Mat& src, cv::Mat& dst, cv::Size kernel, int offset, bool isLight = true);
    //    /**
    //     * @brief 图像逆时针旋转
    //     * @param matSrc     [in]    输入原图
    //     * @param angle       [in]    旋转角度
    //     * @return 旋转后的图片
    //     */
    //    static Mat imgRotate(Mat matSrc, float angle);
        /**
         * @brief 图像以一点逆时针旋转
         * @param src     [in]    输入原图
         * @param center     [in]    旋转基点
         * @param angle       [in]    旋转角度
         * @return 旋转后的图片
         */
    static cv::Mat imgRotate2(cv::Mat src, cv::Point2f center, float angle);
    /**
     * @brief 图像放射变换
     * @param src     [in]    输入原图
     * @param center     [in]    变换基点
     * @param angle       [in]    旋转角度
     * @param dltX       [in]    x方向移动
     * @param dltY       [in]    y方向移动
     * @return mat变换后的图片
     */
    static cv::Mat affineTransform(cv::Mat& src, cv::Point2f center, float angle, float dltX, float dltY);
    /**
     * @brief 俩条直线的交点
     * @param k1     [in]    直线1的k值
     * @param b1     [in]    直线1的b值
     * @param k2     [in]    直线2的k值
     * @param b2     [in]    直线2的b值
     * @param point       [out]    交点
     * @return 函数执行成功与否
     *         0：成功
     *         1：无交点
     */
    static int intersectionPoint(float k1, float b1, float k2, float b2, QPointF& point);
    /*!
     * \brief Qimage类型图片转Hobject
     * \param src [in] 输入Hobject图像
     * \return    [out] 输出HImage
     */
     //    static Hobject qimageToHImge(QImage &src);
     //    /*!
     //     * \brief qimagesToMats QImages转Mats
     //     * \param qimgs [in] 输入QImage图像集
     //     * \param mimgs [out] 输出Mat类图像集
     //     */
     //    static void qimagesToMats(vector<QImage> &qimgs, vector<Mat> &mimgs);
    static zyhn_core::CResult bayerToRgb(unsigned char* srcRaw, int bufferSize, QImage& dst, int mode, int width, int height);
};


#endif // !ALGORITHM_H