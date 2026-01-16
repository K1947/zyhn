#ifndef GLOBALVAR_MANEGER
#define GLOBALVAR_MANEGER
#include "zyhn_comm_api_global.h"

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QJsonObject>

struct GlobalVar
{
    bool varType; //真为接收（设备-》视觉），假为发送（视觉-》设备）
    QString varName;
    QString remark;
    int dataType; // 数据类型，使用qMetaTypeId<qint8>() 赋值
    QVariant defaultValue; // 默认值
    QVariant attentionValue; //关注值

    GlobalVar()
    {
        varType = false;
        varName = "";
        remark = "";
        dataType = 0;
        defaultValue = QVariant();
        attentionValue = QVariant();
    }
    GlobalVar(bool var_type, QString var_name, QString var_remark, int var_Datatype, QVariant var_defaultV, QVariant var_attentionV = QVariant())
    {
        varType = var_type;
        varName = var_name;
        remark = var_remark;
        dataType = var_Datatype;
        defaultValue = var_defaultV;
        attentionValue = var_attentionV;
    }
};

struct GlobalVarMap
{
    GlobalVar globalVar;
    QVariant* varAddr;
    QString deviceAddr;
    QJsonObject ToJsonObject() const
    {
        QJsonObject jsonObj;
        jsonObj["remark"] = globalVar.remark;
        jsonObj["varName"] = globalVar.varName;
        jsonObj["varType"] = globalVar.varType;
        jsonObj["dataType"] = globalVar.dataType;
        jsonObj["defaultValue"] = globalVar.defaultValue.toJsonValue();
        jsonObj["attentionValue"] = globalVar.attentionValue.toJsonValue();
        jsonObj["deviceAddr"] = deviceAddr;
        return jsonObj;
    }
    static GlobalVarMap FromJsonObject(const QJsonObject& jsonObj)
    {
        GlobalVarMap var;
        var.globalVar.remark = jsonObj["remark"].toString();
        var.globalVar.varName = jsonObj["varName"].toString();
        var.globalVar.varType = jsonObj["varType"].toBool();
        var.globalVar.dataType = jsonObj["dataType"].toInt();
        var.globalVar.defaultValue = jsonObj["defaultValue"].toVariant();
        var.globalVar.attentionValue = jsonObj["attentionValue"].toVariant();
        var.deviceAddr = jsonObj["deviceAddr"].toString();
        return var;
    }
};

struct GlobalVarMapTable //全局变量到通信设备地址的映射
{
    int mapTableName; // 变量表名称
    QMap<QString, GlobalVarMap>  mapTable;
};

class ZYHN_COMM_API_EXPORT GlobalVarManager : public QObject
{
    Q_OBJECT
public:
    static GlobalVarManager* m_instance;
    static GlobalVarManager* getInstance();
    ~GlobalVarManager();

    //全局变量管理
    /**
     * @brief 添加全局变量
     * @param addrTableName 地址表名称，int型（高16位产品ID，低16位工位ID）
     * @param var 全局变量信息
     * @return 若返回flase，则该变量在该工位下已存在
     */
    bool addGlobalVar(const int& addrTableName, const GlobalVar& var);
    void removeGlobalVar(const int& addrTableName, const GlobalVar& var);
    QStringList getGlobalVarNames(const int& addrTableName);
    QVariant* getGlobalVar(const int& addrTableName, const QString& var);

    //外围通信设备地址映射管理
    GlobalVarMapTable getMapTable(const int& tableName);
    QStringList getMapNames(const int& tableName);
    bool saveMapTable(const int& workstationID, const GlobalVarMapTable& mapTable);
private:
    GlobalVarManager(QObject* parent = nullptr);

    QMap<int, GlobalVarMapTable> m_mapTbale;

private slots:
    void onCurrentProductChanged();
signals:
    void currentProductChanged();//当前产品改变

    void signalShowLog(const QString& log);
};

#endif // GLOBALVAR_MANEGER