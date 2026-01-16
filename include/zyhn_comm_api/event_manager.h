#ifndef EVENT_MANAGER
#define EVENT_MANAGER
#include "zyhn_comm_api_global.h"
#include "globalvar_manager.h"

#include <QObject>
#include <QJsonObject>
#include <QTimer>

struct CommunicationEvent
{
    QString name;
    QString communication_device; //通信设备连接名
    QString workstation_name; //工位名，用于选择全局变量表
    bool type;//真表示读取外围设备，控制本地程序；假表示读取本地程序，控制外围设备
    QList<QString> inputData;
    QList<QString> outputData;
    QJsonObject ToJsonObject() const
    {
        QJsonObject jsonObj;
        jsonObj["name"] = name;
        jsonObj["communication_device"] = communication_device;
        jsonObj["workstation_name"] = workstation_name;
        jsonObj["type"] = type;
        jsonObj["inputData"] = inputData.join(",");
        jsonObj["outputData"] = outputData.join(",");
        return jsonObj;
    }
    static CommunicationEvent FromJsonObject(const QJsonObject& jsonObj)
    {
        CommunicationEvent event;
        event.name = jsonObj["name"].toString();
        event.communication_device = jsonObj["communication_device"].toString();
        event.workstation_name = jsonObj["workstation_name"].toString();
        event.type = jsonObj["type"].toBool();
        event.inputData = jsonObj["inputData"].toString().split(",");
        event.outputData = jsonObj["outputData"].toString().split(",");
        return event;
    }
};

class ZYHN_COMM_API_EXPORT EventManager : public QObject
{
    Q_OBJECT
public:
    static EventManager* m_instance;
    static EventManager* getInstance();
    ~EventManager();
    bool checkEventIsValid(QStringList& errorTips);

    void startEvent();
    void stopEvent();
    void resetEvent();

    //若存在，则更新事件，不存在则插入事件
    bool updateCommunicationEvent(const QString& name, const CommunicationEvent& event);
    void removeCommunicationEvent(const QString& name);
    CommunicationEvent getCommunicationEvent(const QString& name);
    QMap<QString, CommunicationEvent> getAllCommunicationEvent();
    QStringList getCommunicationEventName();

public slots:
    void onCurrentProductChanged();
    void onEventExec();

private:
    bool readInputFromDevice(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
    bool readInputFromLocal(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
    bool writeOutputToDevice(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
    bool writeOutputToLocal(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
    void resetDevice(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
    void resetLocal(CommunicationEvent event, GlobalVarMapTable addrMapTbale);
private:
    EventManager(QObject* parent = nullptr);

    QMap<QString, CommunicationEvent> m_eventMap;

    QTimer event_exec_timer_;
signals:
    void currentProductChanged();//当前产品改变
    void signalShowLog(const QString& log);

    void eventTrigger(const int& tableNmae, const QStringList triggerPara);    
};

#endif  //EVENT_MANAGER

