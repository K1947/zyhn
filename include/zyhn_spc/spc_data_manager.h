#ifndef SPC_DATA_MANAGER_H
#define SPC_DATA_MANAGER_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <QMutex>
#include <QtConcurrent>
#include "algorithm_context.h"
#include "spc_types.h"
#include "zyhn_spc_global.h"

class ZYHN_SPC_EXPORT SpcDataManager : public QObject 
{
    Q_OBJECT
public:
    static SpcDataManager* instance();
    void processResult(const AlgorithmContext& context);
    SpcStatistics getStatistics(const QString& stationId) const;
    void clearResult();
signals:
    void newPointAdded(SpcMeasurement data);
    void statsUpdated(QString stationId, SpcStatistics stats);
    void alarmTriggered(QString message);
    void processResultSignals(int idx, bool isok);
    void clearResultSignals();

private:
    explicit SpcDataManager(QObject* parent = nullptr);
    void calculateStats(const QString& stationId);
    // void saveToDb(const SpcMeasurement& data);     // 持久化
    static SpcDataManager* instance_;
    QMap<QString, QVector<SpcMeasurement>> m_historyData;
    QMap<QString, SpcStatistics> m_statsMap;
    mutable QMutex m_mutex;
};
#endif
