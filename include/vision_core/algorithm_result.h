#ifndef ALGORITHM_RESULT_H
#define ALGORITHM_RESULT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QImage>
#include "vision_core_global.h"

enum class ResultType :int;
class ResultShape;
class VISION_CORE_EXPORT AlgorithmResult
{
public:
    AlgorithmResult();
    ~AlgorithmResult();

    void addOutputImage(const QImage& image);

    const QVector<QImage>& outputImages() const;
    const QVariantMap& metaData() const;
    const QVector<std::shared_ptr<ResultShape>> resultShapes() const;

    int code() const;
    void setCode(int value);
    void setMsg(QString);
    bool isSuccess() const;
    QString msg() const;
    QString algoName() const;
    int imageIdx() const;
    void setImageIdx(int);
    void setAlgoName(QString name);
    void addMetaData(const QString& key, const QVariant& value);
    void addResultShape(std::shared_ptr<ResultShape> shape);
    void setResultShapes(QVector<std::shared_ptr<ResultShape>> shapes);
    void setResultType(ResultType);
    ResultType resultType() const;

private:
    QString algo_name_;
    int result_img_idx_; // 结果索引
    QVector<QImage> output_images_;
    QVariantMap meta_data_;
    QString msg_;
    ResultType result_type_; // OK or NG
    int code_ = 0;
    QVector<std::shared_ptr<ResultShape>> result_shapes_;
};

#endif // !ALGORITHM_RESULT_H