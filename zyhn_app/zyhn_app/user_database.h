#ifndef USER_DATABASE_H
#define USER_DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>
#include "user_models.h"

class UserDatabase : public QObject
{
    Q_OBJECT

public:
    explicit UserDatabase(QObject *parent = nullptr);
    ~UserDatabase();

    // 数据库初始化
    bool initializeDatabase();
    bool createTables();
    bool createDefaultSuperAdmin();
    
    // 用户管理
    bool addUser(const UserInfo& user);
    bool updateUser(const UserInfo& user);
    bool deleteUser(int userId);
    bool deleteUser(const QString& username);
    UserInfo getUserById(int userId);
    UserInfo getUserByUsername(const QString& username);
    QList<UserInfo> getAllUsers();
    QList<UserInfo> getUsersByRole(UserRole role);
    
    // 用户验证
    bool validateUser(const QString& username, const QString& password);
    bool updateLastLoginTime(const QString& username);
    
    // 权限管理
    bool updateUserPermissions(int userId, int permissions);
    bool updateUserRole(int userId, UserRole role);
    
    // 用户状态管理
    bool activateUser(int userId);
    bool deactivateUser(int userId);
    bool isUserActive(const QString& username);
    
    // 数据库维护
    bool backupUsers();
    bool restoreUsers();
    void closeDatabase();

signals:
    void userAdded(const UserInfo& user);
    void userUpdated(const UserInfo& user);
    void userDeleted(int userId);
    void databaseError(const QString& error);

private:
    QSqlDatabase db_;
    QString dbPath_;
    
    // 私有方法
    bool openDatabase();
    bool executeQuery(const QString& query);
    UserInfo userFromQuery(const QSqlQuery& query);
    QString encryptPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& hash);
    QString generateSalt();
    QString hashPassword(const QString& password, const QString& salt);
};

#endif // USER_DATABASE_H
