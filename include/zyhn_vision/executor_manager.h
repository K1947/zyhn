#ifndef EXECUTOR_MANAGER_H
#define EXECUTOR_MANAGER_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include "zyhn_core.h"
#include "zyhn_vision_global.h"

class StationExecutor;
class StationBridge;
class ZYHN_VISION_EXPORT ExecutorManager : public QObject {
    Q_OBJECT
public:
    static ExecutorManager* instance();
    void initialize(zyhn_core::IApplication* app);
    void setStationAlgo();
    std::shared_ptr<StationExecutor> addStationExecutor(zyhn_core::IApplication* app, int id, const QJsonArray& algoConfigs);
    std::shared_ptr<StationExecutor> getExecutor(int id);
    void removeExecutor(int id);
    void clearExecutor();
    void bindStationParameters(int wid);
public slots:
    void onExternalTriggerReceived(int, const QVariantMap& params);
    void onProductSwitched();
signals:
    void stationMessage(int stationId, int type, QString msg);

private:
    explicit ExecutorManager(QObject* parent = nullptr);
    static ExecutorManager* instance_;

    QMap<int, std::shared_ptr<StationExecutor>> executors_;
    QMap<int, std::shared_ptr<StationBridge>> bridges_;
    zyhn_core::IApplication* app_;
    // 存储所有的通信协议源（PLC, TCP, Sensor等）
    // QList<TriggerSource*> m_triggerSources;
};

#endif
