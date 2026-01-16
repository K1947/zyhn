#ifndef ALGORITHM_UTILS_HALCON
#define ALGORITHM_UTILS_HALCON
#include <halconcpp/HalconCpp.h>
#include <iostream>
#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "algorithm_global.h"

using namespace HalconCpp;


class ALGORITHM_EXPORT AlgorithmUtilsHalcon : public QObject
{
    Q_OBJECT
public:
    AlgorithmUtilsHalcon();

    /**
     * @brief Qimage类型图片转Hobject
     * @param src     [in]    输入QImage图像
     * @param himage   [out]    输出Hobject图像
     * @return 返回int  0：函数正常，1：参数异常
    */
    static QImage hobjectToQimage(const HalconCpp::HObject hobject);
    static HalconCpp::HObject qimageToHobject(const QImage src);
    // cv::Mat转换为Hobject
    static int matToHobject(const cv::Mat image, HObject& himage);
    // Hobject转换为cv::Mat
    static int HobjectToMat(HObject H_img, cv::Mat& mImg);
};

#endif // !ALGORITHM_UTILS_HALCON