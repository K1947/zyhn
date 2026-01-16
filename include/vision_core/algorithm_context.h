#ifndef ALGORITHM_CONTEXT_H
#define ALGORITHM_CONTEXT_H

#include <QVariantMap>
#include <QVector>
#include <QSharedPointer>
#include <QMutex>
#include "algorithm_input.h"
#include "algorithm_result.h"
#include "vision_core_global.h"

/**
 * 算法执行上下文（数据总线）
 * 贯穿整个流水线，负责存储输入数据、中间过程数据和各步骤结果
 */
class VISION_CORE_EXPORT AlgorithmContext {
public:
    explicit AlgorithmContext(const AlgorithmInput& initialInput);

    AlgorithmContext(const AlgorithmContext& other);
    AlgorithmContext& operator=(const AlgorithmContext& other);
    ~AlgorithmContext();

    AlgorithmInput input;

    void setData(const QString& key, const QVariant& value);
    QVariant getData(const QString& key, const QVariant& defaultValue = QVariant()) const;
    bool hasData(const QString& key) const;

    int displayPos(int idx) const;
    void setDisplayPos(int idx, int pos);
    void setDisplayMap(QMap<int, int>&);

    int workNo() const;
    void setWorkNo(int no);

    QVariantMap dataBus() const;

    QVector<AlgorithmResult> stepResults;

    AlgorithmResult lastResult() const;
    void clear();

private:
    struct PrivateData;
    QSharedPointer<PrivateData> d;
};

#endif // ALGORITHM_CONTEXT_H
