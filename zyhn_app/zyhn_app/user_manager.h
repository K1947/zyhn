#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include "zyhn_core.h"
#include <QObject>
#include <QVector>
#include <QString>
#include "data_managers.h"

using namespace zyhn_core;
#define ZYHN_SUPER_ADMIN_NAME "admin"

class UserManager : public IUserManager
{
    Q_OBJECT

public:
    explicit UserManager(QObject *parent = nullptr);
    ~UserManager() override;

    bool initWithApp(IApplication* app) override;
    bool login(const QString& username, const QString& password) override;
    void logout() override;
    bool isLoggedIn() const override;
    UserInfo getCurrentUser() const override;
    void refreshUserList() override;
    QVector<UserInfo> userList() const override;
    bool addUser(const UserInfo& user) override;
    bool updateUser(const UserInfo& user) override;
    bool removeUser(int userId) override;
    UserInfo findUserById(int userId) const override;
    UserInfo findUserByUsername(const QString& username) const override;

    bool hasPermission(Permission permission) const override;
    bool canManageUsers() const override;
    void setLoginUser();

private:
    UserInfo getSuperAdminAccount();
    int getNextUserId();
    bool validateUserInput(const UserInfo& user);
    QString getRoleName(UserRole role) const;
    int getRolePermissions(UserRole role) const;    

private:
    QVector<UserInfo> users_;
    UserInfo currentUser_;
    UserInfo superAdmin_;
    bool isLoggedIn_;
    int nextUserId_;
    UserDataManager* udb_;
};

#endif