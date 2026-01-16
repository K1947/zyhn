// plc_communication.h
#ifndef PLC_COMMUNICATION_H
#define PLC_COMMUNICATION_H

#include "communication_core.h"
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QMap>

#include "Snap7/snap7.h"

// PLC数据类型
enum class PlcDataType
{
    BOOL,
    BYTE,
    WORD,
    DWORD,
    INT,
    DINT,
    REAL,
    STRING
};

// PLC地址信息
struct PlcAddress {
    QString area;       // DB, M, I, Q, T, C 等
    int dbNumber;       // DB块号
    int byteOffset;     // 字节偏移
    int bitOffset;      // 位偏移 (0-7)
    PlcDataType dataType;
    QString saveType;  // 存储数据顺序
    int length;         // 数据长度 (用于字符串和数组)

    PlcAddress()
    {
        dataType  = PlcDataType::BOOL;
        dbNumber = 1;
        byteOffset = 0;
        bitOffset = 0;
        length = 1;
    }
};

// PLC数据项
struct PlcDataItem {
    QString name;
    PlcAddress address;
    QVariant value;
    QDateTime lastUpdate;
    bool isConnected;
};

// PLC通信结果
struct PlcReadResult {
    bool success;
    QVariant value;
    QString error;
    QDateTime timestamp;
};

struct PlcWriteResult {
    bool success;
    QString error;
    QDateTime timestamp;
};

class ZYHN_COMM_API_EXPORT PlcCommunication : public ICommunication
{
    Q_OBJECT
public:
    explicit PlcCommunication(CommunicationType type, QObject* parent = nullptr);
    ~PlcCommunication();

    // ICommunication interface
    bool initialize(const CommunicationConfig& config) override;
    bool connect() override;
    void disconnect() override;
    bool reconnect() override;

    CommunicationStatus getStatus() const override;
    bool isConnected() const override;
    QString getLastError() const override;

    QByteArray readData(int timeout = 3000) override;
    bool writeData(const QByteArray& data) override;
    QByteArray sendAndReceive(const QByteArray& data, int timeout = 3000) override;

    void updateConfig(const CommunicationConfig& config) override;
    CommunicationConfig getConfig() const override;

    quint64 getBytesRead() const override;
    quint64 getBytesWritten() const override;
    QDateTime getLastCommunicationTime() const override;

    // PLC专用方法
    // 单点读写
    PlcReadResult readBit(const QString& address);
    PlcReadResult readByte(const QString& address);
    PlcReadResult readWord(const QString& address);
    PlcReadResult readDWord(const QString& address);
    PlcReadResult readInt(const QString& address);
    PlcReadResult readDInt(const QString& address);
    PlcReadResult readReal(const QString& address);
    PlcReadResult readString(const QString& address, int length = 256);

    PlcWriteResult writeBit(const QString& address, bool value);
    PlcWriteResult writeByte(const QString& address, quint8 value);
    PlcWriteResult writeWord(const QString& address, quint16 value);
    PlcWriteResult writeDWord(const QString& address, quint32 value);
    PlcWriteResult writeInt(const QString& address, qint16 value);
    PlcWriteResult writeDInt(const QString& address, qint32 value);
    PlcWriteResult writeReal(const QString& address, float value);
    PlcWriteResult writeString(const QString& address, const QString& value);

    // 批量读写
    //批量读取是根据地址确定类型，无法区分字节数相同（寻址方式一致）的类型，如real和dint，bool和string等
    QMap<QString, PlcReadResult> readMultiple(const QList<QString>& addresses);
    QMap<QString, PlcWriteResult> writeMultiple(const QMap<QString, QVariant>& values);

    // 数据监控
    void addDataItem(const QString& name, const QString& address, PlcDataType dataType);
    void removeDataItem(const QString& name);
    void startMonitoring(int interval = 1000);
    void stopMonitoring();
    QMap<QString, PlcDataItem> getDataItems();

    // PLC控制
    bool startPlc();
    bool stopPlc();
    bool resetPlc();
    PlcReadResult getPlcStatus();

    // 诊断信息
    QVariantMap getDiagnosticInfo() const;
    QVector<QString> getErrorLog();

private slots:
    void onHeartbeat();
    void onMonitorTimeout();
    void onConnectionCheck();
    void attemptReconnect();

private:
    // 协议特定实现
    bool connectS7();
    bool connectMitsubishi();
    void disconnectPlc();

    // S7协议实现
    QByteArray s7ReadData(const PlcAddress& address, int length);
    bool s7WriteData(const PlcAddress& address, const QByteArray& data);

    //// COTP协议实现
    //QByteArray buildCOTPConnectionRequest();
    //bool parseCOTPConnectionResponse(const QByteArray& response);
    //QByteArray buildS7CommunicationRequest(int rack, int slot);
    //bool parseS7CommunicationResponse(const QByteArray& response);
    //
    // MC协议实现
    QByteArray buildMCProtocolTestRequest(int stationNo);
    bool parseMCProtocolTestResponse(const QByteArray& response);
    QByteArray mitsubishiReadData(const PlcAddress& address, int length);
    bool mitsubishiWriteData(const PlcAddress& address, const QByteArray& data);
    QByteArray buildMCReadRequest(const PlcAddress& address, int length);
    QByteArray buildMCWriteRequest(const PlcAddress& address, const QByteArray& data);
    QByteArray parseMCReadResponse(const QByteArray& response);
    bool parseMCWriteResponse(const QByteArray& response);

    // 通用方法
    PlcAddress parseAddress(const QString& addressStr);
    QByteArray convertToBytes(const QVariant& value, const PlcAddress& address);
    QVariant convertFromBytes(const QByteArray& data, const PlcAddress& address);
    int getDataTypeSize(PlcDataType dataType);
    int getAreaCode(const QString& area);
    int getDataTypeCode(PlcDataType dataType);

    void logError(const QString& error);
    PlcWriteResult writeAddress(const PlcAddress& address, const QVariant& value);
    PlcReadResult readAddress(const PlcAddress& address);
    void startHeartbeat();
    void stopHeartbeat();

private:
    CommunicationType m_type;
    CommunicationConfig m_config;
    CommunicationStatus m_status;
    QString m_lastError;

    // 网络通信
    TS7Client* m_snap7_client_;
    QMutex m_connectionMutex;

    // 通信组件
    QTimer* m_heartbeatTimer;
    QTimer* m_monitorTimer;
    QTimer* m_connectionTimer;
    QTimer* m_reconnectTimer;

    // 数据管理
    QMap<QString, PlcDataItem> m_dataItems;
    QMutex m_dataMutex;

    // 统计信息
    quint64 m_bytesRead;
    quint64 m_bytesWritten;
    QDateTime m_lastCommTime;

    // 错误日志
    QVector<QString> m_errorLog;
    QMutex m_errorMutex;

    // 协议特定数据
    QMap<QString, QVariant> m_protocolData;

    // 连接状态和配置
    bool m_plcConnected;
    int m_connectionRetries;
    int m_maxRetries;
    int m_connectionTimeout;


public:
    static QString DB_Area_Name;
    static QString EB_Area_Name;
    static QString AB_Area_Name;
    static QString MB_Area_Name;
};

#endif // PLC_COMMUNICATION_H