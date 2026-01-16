// communication_manager.h
#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include "communication_core.h"
#include "communication_factory.h"
#include <QObject>
#include <QMap>
#include <QString>
#include <QMutex>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include "zyhn_comm_api_global.h"
#include "product_info.h"
#include "zyhn_core.h"

// 通信实例配置
struct CommunicationInstance {
    QString name;
    CommunicationType type;
    CommunicationConfig config;
    ICommunication* communication;
    bool autoConnect;
    bool isConnected;
    QDateTime createTime;
    QDateTime lastActivity;
};


class ZYHN_COMM_API_EXPORT CommunicationManager : public QObject
{
    Q_OBJECT

public:
    CommunicationManager(const CommunicationManager&) = delete;
    CommunicationManager& operator=(const CommunicationManager&) = delete;

    explicit CommunicationManager(QObject* parent = nullptr);
    ~CommunicationManager();

    static CommunicationManager* getInstance();

    // 通信实例管理
    bool createCommunication(const QString& name, CommunicationType type,
        const CommunicationConfig& config, bool autoConnect = true);
    bool removeCommunication(const QString& name);
    bool connectCommunication(const QString& name);
    bool disconnectCommunication(const QString& name);
    bool reconnectCommunication(const QString& name);
    QString findInstanceName(ICommunication* comm) const;
    QString getCommTypeName(CommunicationType type) const;

    // 批量操作
    bool connectAll();
    bool disconnectAll();
    bool reconnectAll();

    // 实例访问
    ICommunication* getCommunication(const QString& name);
    CommunicationInstance getCommunicationInfo(const QString& name) const;
    QStringList getCommunicationNames() const;
    QList<CommunicationInstance> getAllCommunications() const;

    // 状态查询
    bool isConnected(const QString& name) const;
    CommunicationStatus getStatus(const QString& name) const;
    int getConnectedCount() const;
    int getTotalCount() const;

    // 配置管理
    bool updateConfig(const QString& name, const CommunicationConfig& config);
    bool saveConfiguration(const CommunicationConfig& config);
    void removeConfiguration(const QString& name);
    bool loadConfiguration(const QString& filename);

    // 数据读写
    QByteArray readData(const QString& name, int timeout = 3000);
    bool writeData(const QString& name, const QByteArray& data);
    QByteArray sendAndReceive(const QString& name, const QByteArray& data, int timeout = 3000);

    // 统计信息
    QVariantMap getStatistics(const QString& name) const;
    QVariantMap getAllStatistics() const;
    void resetStatistics(const QString& name);
    void resetAllStatistics();

    // 监控功能
    void startMonitoring(int interval = 5000);
    void stopMonitoring();
    bool isMonitoring() const;

    QVector<PortInfo> validPorts();

    void initCommunicationManager(zyhn_core::IApplication* app);
    int getProductID();
    QStringList getAllWorkStationNames();
    int getWorkStationID(const QString& workstationName);
    int getWorkID(const QString& workstationName);
private slots:
    //初始化通信模块
    void onCurrentProductChanged();
    void onAddProduct(int id);
    void onDeleteProduct(int id);
signals:
    void communicationCreated(const QString& name);
    void communicationRemoved(const QString& name);
    void communicationConnected(QString name);
    void communicationDisconnected(QString name);
    void communicationError(const QString& name, const QString& error);
    void dataReceived(const QString& name, const QByteArray& data);
    void statusChanged(const QString& name, CommunicationStatus status);
    void currentProductChanged();//当前产品改变,用于通知界面
    void initVarManager();
    void initEventManager();

    // 全局状态信号
    void connectedCountChanged(int count);
    void monitoringStarted();
    void monitoringStopped();
private slots:
    void onCommunicationDataReceived(const QByteArray& data);
    void onCommunicationStatusChanged(CommunicationStatus status);
    void onCommunicationError(const QString& error);
    void onConnectionEstablished();
    void onConnectionLost();
    void onMonitoringTimeout();

private:
    void setupSignalSlots(ICommunication* comm, const QString& name);
    void removeSignalSlots(ICommunication* comm);
    void updateInstanceActivity(const QString& name);
    void cleanupDisconnected();
    QJsonObject instanceToJson(const CommunicationInstance& instance) const;
    CommunicationInstance jsonToInstance(const QJsonObject& json) const;

private:
    QMap<QString, CommunicationInstance> m_instances;
    mutable QMutex m_mutex;
    QTimer* m_monitorTimer;
    static CommunicationManager* m_instance;

    // 监控数据
    QMap<QString, QDateTime> m_lastActivityMap;
    int m_monitoringInterval;

    //app信息
    zyhn_core::IApplication* app_;
    int m_lastProduct;
};

#endif // COMMUNICATION_MANAGER_H