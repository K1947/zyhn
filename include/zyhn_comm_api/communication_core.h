#ifndef COMMUNICATION_CORE_H
#define COMMUNICATION_CORE_H
#include "zyhn_comm_api_global.h"

#include <QObject>
#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>

enum class CommunicationType {
    MODBUS_TCP,
    MODBUS_RTU,
    SERIAL,
    TCP_CLIENT,
    TCP_SERVER,
    PLC_S7,
    PLC_MITSUBISHI
};

// 通信状态枚举
enum class CommunicationStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    CONNECTERROR
};

// 通信配置结构
struct CommunicationConfig {
    QString name;
    CommunicationType type;
    QVariantMap params;
    int timeout = 5000;
    int retryCount = 3;
    int heartbeatInterval = 30000;

    QJsonObject ToJsonObject() const
    {
        QJsonObject jsonObj;
        jsonObj["name"] = name;
        jsonObj["type"] = (int)type;
        QJsonObject paramsObject;
        for (auto it = params.begin(); it != params.end(); it++)
        {
            paramsObject.insert(it.key(), QJsonValue::fromVariant(it.value()));
        }
        jsonObj["params"] = paramsObject;
        jsonObj["timeout"] = timeout;
        jsonObj["retryCount"] = retryCount;
        jsonObj["heartbeatInterval"] = heartbeatInterval;
        return jsonObj;
    }
    static CommunicationConfig FromJsonObject(const QJsonObject& jsonObj)
    {
        CommunicationConfig conf;
        conf.name = jsonObj["name"].toString();
        conf.type = (CommunicationType)jsonObj["type"].toInt();        
        QJsonObject paramsObject = jsonObj["params"].toObject();
        conf.params = paramsObject.toVariantMap();
        conf.timeout = jsonObj["timeout"].toInt();
        conf.retryCount = jsonObj["retryCount"].toInt();
        conf.heartbeatInterval = jsonObj["heartbeatInterval"].toInt();
        return conf;
    }
};

struct PortInfo {
    QString name;
    QString desc;
    bool    busy = false;
};

class ZYHN_COMM_API_EXPORT ICommunication : public QObject
{
    Q_OBJECT

public:
    explicit ICommunication(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ICommunication() = default;

    // 核心接口方法
    virtual bool initialize(const CommunicationConfig& config) = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool reconnect() = 0;

    // 状态查询
    virtual CommunicationStatus getStatus() const = 0;
    virtual bool isConnected() const = 0;
    virtual QString getLastError() const = 0;

    // 数据读写
    virtual QByteArray readData(int timeout = 3000) = 0;
    virtual bool writeData(const QByteArray& data) = 0;
    virtual QByteArray sendAndReceive(const QByteArray& data, int timeout = 3000) = 0;

    // 配置管理
    virtual void updateConfig(const CommunicationConfig& config) = 0;
    virtual CommunicationConfig getConfig() const = 0;

    // 统计信息
    virtual quint64 getBytesRead() const = 0;
    virtual quint64 getBytesWritten() const = 0;
    virtual QDateTime getLastCommunicationTime() const = 0;

signals:
    void dataReceived(const QByteArray& data);
    void statusChanged(CommunicationStatus status);
    void errorOccurred(const QString& error);
    void connectionEstablished();
    void connectionLost();
    void dataWritten(const QByteArray& data);
};

#endif // COMMUNICATION_INTERFACE_H