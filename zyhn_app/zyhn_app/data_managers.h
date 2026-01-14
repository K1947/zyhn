#ifndef DATA_MANAGERS_H
#define DATA_MANAGERS_H

#include <QObject>
#include "zyhn_core.h"

using namespace zyhn_core;

class ConfigDataManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigDataManager(IAppDatabase* db, QObject* parent = nullptr);

    bool saveConfig(const QString& key, const QVariant& value);
    QVariant getConfig(const QString& key, const QVariant& defaultValue = QVariant());
    bool deleteConfig(const QString& key);
    QMap<QString, QVariant> loadAllConfigs();

private:
    IAppDatabase* db_;
    static const QString TABLE_NAME;
};

class UserDataManager : public QObject
{
    Q_OBJECT
public:
    explicit UserDataManager(IAppDatabase* db, QObject* parent = nullptr);

    bool saveUser(const UserInfo& user);
    bool deleteUser(int id);
    QVector<UserInfo> loadAllUsers();
    UserInfo findUserById(int id);
    UserInfo findUserByUsername(const QString& username);
    bool validateUser(const QString& username, const QString& password);
    int getNextUserId();

private:
    QString serializeUser(const UserInfo& user);
    UserInfo deserializeUser(const QString& json);    
    IAppDatabase* db_;
    static const QString TABLE_NAME;
};

#endif // DATA_MANAGERS_H

