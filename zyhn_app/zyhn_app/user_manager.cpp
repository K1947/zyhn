#include "user_manager.h"
#include <QDebug>
#include <QDateTime>
#include <QRegularExpression>
#include "utils.h"
#include "permission_macros.h"

UserManager::UserManager(QObject *parent)
    : IUserManager(parent)
    , isLoggedIn_(false)
    , nextUserId_(1)
    , udb_(nullptr)
{
    superAdmin_ = getSuperAdminAccount();
}

UserManager::~UserManager()
{
    if (udb_) {
        delete udb_;
    }
}

bool UserManager::initWithApp(IApplication* app)
{
    IManager::initWithApp(app);
    qDebug() << "UserManager initialized with application";

    udb_ = new UserDataManager(app->appDb(), this);

    return true;
}

bool UserManager::login(const QString& username, const QString& password)
{
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "用户名或密码为空";
        return false;
    }

    if (username == ZYHN_SUPER_ADMIN_NAME) {
        if (Utils::encryptStr(password) == superAdmin_.password) {
            currentUser_ = superAdmin_;
            isLoggedIn_ = true;
            application()->settings()->setValue("general\\login_name", username);
            emit userLoggedIn(currentUser_.id);
            return true;
        }
    }

    UserInfo user = findUserByUsername(username);
    if (user.id == -1) {
        qDebug() << "用户不存在:" << username;
        return false;
    }

    if (Utils::encryptStr(password) != user.password) {
        qDebug() << "密码错误:" << username;
        return false;
    }

    currentUser_ = user;
    isLoggedIn_ = true;

    application()->settings()->setValue("general\\login_name", user.username);
    qDebug() << "用户登录成功:" << username;
    emit userLoggedIn(user.id);
    return true;
}

void UserManager::logout()
{
    if (isLoggedIn_) {
        qDebug() << "用户登出:" << currentUser_.username;
        emit userLoggedOut();
        currentUser_ = UserInfo();
        isLoggedIn_ = false;
        application()->settings()->remove("general\\login_name");
    }
}

bool UserManager::isLoggedIn() const
{
    return isLoggedIn_;
}

UserInfo UserManager::getCurrentUser() const
{
    return currentUser_;
}

QVector<UserInfo> UserManager::userList() const
{
    return users_;
}

void UserManager::refreshUserList()
{
   if (udb_) {
      users_ = udb_->loadAllUsers();
   }
}

bool UserManager::addUser(const UserInfo& user)
{
    if (!PermissionUtils::hasPermission(P_UserOperate)) {
        qDebug() << "没有用户管理权限";
        return false;
    }

    if (!validateUserInput(user)) {
        return false;
    }

    // 检查用户名是否已存在
    if (findUserByUsername(user.username).id != -1) {
        qDebug() << "用户名已存在:" << user.username;
        return false;
    }

    UserInfo newUser = user;
    newUser.id = getNextUserId();
    newUser.createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    newUser.password = Utils::encryptStr(newUser.password);
    newUser.permissions = getRolePermissions(user.role);
    udb_->saveUser(newUser);
    refreshUserList();
    qDebug() << "用户添加成功:" << newUser.username;
    emit userAdded(user.id);
    return true;
}

bool UserManager::updateUser(const UserInfo& user)
{
    if (!canManageUsers()) {
        qDebug() << "没有用户管理权限";
        return false;
    }

    // 检查是否尝试修改超级管理员
    UserInfo existingUser = findUserById(user.id);
    if (existingUser.role == UR_SuperAdmin && user.role != UR_SuperAdmin) {
        qDebug() << "不能修改超级管理员角色";
        return false;
    }

    for (auto& u : users_) {
        if (u.id == user.id) {
            u.username = user.username;
            u.realName = user.realName;
            u.role = user.role;
            u.permissions = user.permissions;
            qDebug() << "用户更新成功:" << u.username;
            udb_->saveUser(u);

            emit userUpdated(u.id);
            return true;
        }
    }

    qDebug() << "用户不存在:" << user.id;
    return false;
}

bool UserManager::removeUser(int userId)
{
    if (!canManageUsers()) {
        qDebug() << "没有用户管理权限";
        return false;
    }

    UserInfo user = findUserById(userId);
    if (user.id == -1) {
        qDebug() << "用户不存在:" << userId;
        return false;
    }

    // 不能删除超级管理员
    if (user.role == UR_SuperAdmin) {
        qDebug() << "不能删除超级管理员";
        return false;
    }

    // 不能删除自己
    if (user.id == currentUser_.id) {
        qDebug() << "不能删除自己";
        return false;
    }

    for (int i = 0; i < users_.size(); ++i) {
        if (users_[i].id == userId) {
            users_.removeAt(i);
            qDebug() << "用户删除成功:" << user.username;
            udb_->deleteUser(userId);

            refreshUserList();
            emit userDeleted(userId);
            return true;
        }
    }

    return false;
}

UserInfo UserManager::findUserById(int userId) const
{
    for (const auto& user : users_) {
        if (user.id == userId) {
            return user;
        }
    }
    return UserInfo();
}

UserInfo UserManager::findUserByUsername(const QString& username) const
{
    for (const auto& user : users_) {
        if (user.username == username) {
            return user;
        }
    }
    return UserInfo();
}

bool UserManager::hasPermission(Permission permission) const
{
    if (!isLoggedIn_) {
        return false;
    }

    return (currentUser_.permissions & static_cast<int>(permission)) != 0;
}

bool UserManager::canManageUsers() const
{
    return hasPermission(P_UserOperate);
}

UserInfo UserManager::getSuperAdminAccount()
{
    // 创建默认超级管理员
    UserInfo superAdmin;
    superAdmin.id = getNextUserId();
    superAdmin.username = "admin";
    superAdmin.realName = "系统管理员";
    superAdmin.role = UR_SuperAdmin;
    superAdmin.permissions = getRolePermissions(UR_SuperAdmin);
    superAdmin.password = Utils::encryptStr("123456");

    return superAdmin;
}

int UserManager::getNextUserId()
{
    return nextUserId_++;
}

bool UserManager::validateUserInput(const UserInfo& user)
{
    if (user.username.isEmpty()) {
        qDebug() << "用户名不能为空";
        return false;
    }

    if (user.realName.isEmpty()) {
        qDebug() << "真实姓名不能为空";
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9_]{3,20}$");
    if (!usernameRegex.match(user.username).hasMatch()) {
        qDebug() << "用户名格式不正确（3-20位字母数字下划线）";
        return false;
    }

    return true;
}

QString UserManager::getRoleName(UserRole role) const
{
    switch (role) {
        case UR_SuperAdmin: return "超级管理员";
        case UR_Admin: return "普通管理员";
        case UR_Operator: return "一般操作员";
        default: return "未知角色";
    }
}

void UserManager::setLoginUser()
{
    QString userName = application()->settings()->value("general\\login_name", "").toString();
    if (userName.isEmpty()) {
        return;
    }

    if (userName == ZYHN_SUPER_ADMIN_NAME) {
        this->currentUser_ = getSuperAdminAccount();
        this->isLoggedIn_ = true;
        emit userLoggedIn(this->currentUser_.id);
        return;
    }

    for (UserInfo userinfo : users_) {
        if (userinfo.username == userName) {
            this->currentUser_ = userinfo;
            this->isLoggedIn_ = true;
            emit userLoggedIn(this->currentUser_.id);
            break;
        }
    }
}

int UserManager::getRolePermissions(UserRole role) const
{
    switch (role) {
        case UR_SuperAdmin:
            return static_cast<int>(P_VisionPanel) |
                   static_cast<int>(P_ControlPanel) |
                   static_cast<int>(P_ProductView) |
                   static_cast<int>(P_CommPanel) |
                   static_cast<int>(P_MesPanel) |
                   static_cast<int>(P_DeviceView) |
                   static_cast<int>(P_UserOperate);

        case UR_Admin:
            return static_cast<int>(P_VisionPanel) |
                   static_cast<int>(P_ControlPanel) |
                   static_cast<int>(P_ProductView) |
                   static_cast<int>(P_CommPanel) |
                   static_cast<int>(P_MesPanel) |
                   static_cast<int>(P_DeviceView) |
                   static_cast<int>(P_UserOperate);

        case UR_Operator:
        default:
            return 0;
    }
}
