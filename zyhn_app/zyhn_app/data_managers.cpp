#include "data_managers.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QCryptographicHash>
#include "utils.h"

using namespace zyhn_core;

const QString ConfigDataManager::TABLE_NAME = "app_config";
ConfigDataManager::ConfigDataManager(IAppDatabase* db, QObject* parent)
    : QObject(parent), db_(db)
{
    db_->createTable(TABLE_NAME);
}

bool ConfigDataManager::saveConfig(const QString& key, const QVariant& value)
{
    QJsonObject obj;
    obj["key"] = key;
    obj["value"] = value.toString();
    obj["type"] = QString(value.typeName());
    
    QString json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
        
    // 使用key的hash作为ID
    int id = qHash(key);
    return db_->saveData(TABLE_NAME, id, json);
}

QVariant ConfigDataManager::getConfig(const QString& key, const QVariant& defaultValue)
{
    int id = qHash(key);
    QString json = db_->findDataById(TABLE_NAME, id);
    
    if (json.isEmpty()) {
        return defaultValue;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) return defaultValue;
    
    return doc.object()["value"].toVariant();
}

bool ConfigDataManager::deleteConfig(const QString& key)
{
    int id = qHash(key);
    return db_->deleteData(TABLE_NAME, id);
}

QMap<QString, QVariant> ConfigDataManager::loadAllConfigs()
{
    QMap<QString, QVariant> configs;
    QVector<QString> jsonList = db_->loadAllData(TABLE_NAME);
    
    for (const QString& json : jsonList) {
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QString key = obj["key"].toString();
            QVariant value = obj["value"].toVariant();
            configs[key] = value;
        }
    }
    
    return configs;
}


const QString UserDataManager::TABLE_NAME = "users";

UserDataManager::UserDataManager(IAppDatabase* db, QObject* parent)
    : QObject(parent), db_(db)
{
    db_->createTable(TABLE_NAME);
}

QString UserDataManager::serializeUser(const UserInfo& user)
{
    QJsonObject obj;
    obj["id"] = user.id;
    obj["username"] = user.username;
    obj["password"] = user.password;
    obj["realName"] = user.realName;
    obj["role"] = user.role;
    obj["createTime"] = user.createTime;
    obj["permissions"] = user.permissions;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

UserInfo UserDataManager::deserializeUser(const QString& json)
{
    UserInfo user;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) return user;
    
    QJsonObject obj = doc.object();
    user.id = obj["id"].toInt();
    user.username = obj["username"].toString();
    user.password = obj["password"].toString();
    user.realName = obj["realName"].toString();
    user.createTime = obj["createTime"].toString();
    user.role = static_cast<UserRole>(obj["role"].toInt());
    user.permissions = obj["permissions"].toInt();
    return user;
}

bool UserDataManager::saveUser(const UserInfo& user)
{
    QString json = serializeUser(user);
        
    return db_->saveData(TABLE_NAME, user.id, json);
}

bool UserDataManager::deleteUser(int id)
{
    return db_->deleteData(TABLE_NAME, id);
}

QVector<UserInfo> UserDataManager::loadAllUsers()
{
    QVector<UserInfo> users;
    QVector<QString> jsonList = db_->loadAllData(TABLE_NAME);
    
    for (const QString& json : jsonList) {
        users.append(deserializeUser(json));
    }
    
    return users;
}

UserInfo UserDataManager::findUserById(int id)
{
    QString json = db_->findDataById(TABLE_NAME, id);
    return deserializeUser(json);
}

UserInfo UserDataManager::findUserByUsername(const QString& username)
{
    QString whereClause = QString("username = '%1'").arg(username);
    QVector<QString> jsonList = db_->queryData(TABLE_NAME, whereClause);
    
    if (!jsonList.isEmpty()) {
        return deserializeUser(jsonList.first());
    }
    
    return UserInfo();
}

bool UserDataManager::validateUser(const QString& username, const QString& password)
{
    UserInfo user = findUserByUsername(username);
    if (user.id == 0) return false;
    
    QString hashedPassword = Utils::encryptStr(password);
    return user.password == hashedPassword;
}

int UserDataManager::getNextUserId()
{
    return db_->getNextId(TABLE_NAME);
}

