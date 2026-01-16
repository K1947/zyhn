#ifndef DATA_MANAGERS_H
#define DATA_MANAGERS_H

#include <QObject>
#include "zyhn_core.h"
#include "zyhn_data_global.h"

using namespace zyhn_core;
class ZYHN_DATA_EXPORT UserDataManager : public QObject
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

