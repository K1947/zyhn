#ifndef ALGORITHM_INPUT_H
#define ALGORITHM_INPUT_H

#include <QImage>
#include <QVector>
#include <QVariantMap>
#include <memory>
#include "vision_core_global.h"

// 前置声明
#ifdef USE_OPENCV
namespace cv { class Mat; }
#endif

#ifdef USE_HALCON
namespace HalconCpp { class HObject; }
#endif

class RoiShape;
class AlgorithmInputPrivate;

class VISION_CORE_EXPORT AlgorithmInput
{
public:
    AlgorithmInput();
    ~AlgorithmInput();

    AlgorithmInput(const AlgorithmInput& other);
    AlgorithmInput& operator=(const AlgorithmInput& other);
    AlgorithmInput(AlgorithmInput&& other) noexcept;
    AlgorithmInput& operator=(AlgorithmInput&& other) noexcept;

    void addImage(const QImage& image);
    int imageCount() const;
    void clear();

    QImage getQImage(int index) const;
    QVector<QImage> getQImages() const;


#ifdef USE_OPENCV
    const cv::Mat& getImageAsCvMat(int index) const;
    QVector<cv::Mat> getCvMats() const;
#endif

#ifdef USE_HALCON
    const HalconCpp::HObject& getImageAsHObject(int index) const;
    QVector<HalconCpp::HObject> getHobjets() const;
#endif

    void setRoiShapes(QVector<std::shared_ptr<RoiShape>> shapes);
    QVector<std::shared_ptr<RoiShape>> roiShapes() const;

    QVariantMap context() const;
    void setContext(const QVariantMap& context);
    void addContext(const QString& key, const QVariant& value);
private:
    std::unique_ptr<AlgorithmInputPrivate> d_ptr;
};

#endif // ALGORITHM_INPUT_H