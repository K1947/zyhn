#ifndef SPC_TYPES_H
#define SPC_TYPES_H

#include <QObject>

struct SpcMeasurement {
    QString stationId;      // 工位名称
    QString taskName;       // 任务/算法项名称
    int index;              // 索引
    double value;           // 测量值
    double lsl;             // 下控制限 (Lower Spec Limit)
    double usl;             // 上控制限 (Upper Spec Limit)
    double target;          // 目标值 (Nominal)
    bool isOk;              // 是否合格
    qint64 timestamp;       // 时间戳
};

struct SpcStatistics {
    double mean = 0;        // 平均值
    double stdDev = 0;      // 标准差
    double cpk = 0;         // CPK 过程能力指数
    int totalCount = 0;     // 总数
    int ngCount = 0;        // 不良数
    double yield = 100.0;   // 良率
};

#endif // !SPC_TYPES_H