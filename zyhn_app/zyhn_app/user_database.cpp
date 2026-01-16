#include "user_database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>
#include <QDateTime>

UserDatabase::UserDatabase(QObject *parent)
    : QObject(parent)
{
    // 设置数据库路径
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    dbPath_ = dataPath + "/users.db";
}

UserDatabase::~UserDatabase()
{
    closeDatabase();
}

bool UserDatabase::initializeDatabase()
{
    if (!openDatabase()) {
        return false;
    }
    
    if (!createTables()) {
        return false;
    }
    
    // 检查是否有用户，如果没有则创建默认超级管理员
    QSqlQuery query(db_);
    query.prepare("SELECT COUNT(*) FROM users");
    if (query.exec() && query.next()) {
        int userCount = query.value(0).toInt();
        if (userCount == 0) {
            return createDefaultSuperAdmin();
        }
    }
    
    return true;
}

bool UserDatabase::openDatabase()
{
    db_ = QSqlDatabase::addDatabase("QSQLITE", "UserDatabase");
    db_.setDatabaseName(dbPath_);
    
    if (!db_.open()) {
        qDebug() << "无法打开用户数据库:" << db_.lastError().text();
        emit databaseError("无法打开用户数据库: " + db_.lastError().text());
        return false;
    }
    
    return true;
}

bool UserDatabase::createTables()
{
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            real_name TEXT,
            role INTEGER NOT NULL,
            permissions INTEGER NOT NULL,
            create_time TEXT NOT NULL,
            last_login_time TEXT,
            is_active INTEGER NOT NULL DEFAULT 1,
            created_by TEXT,
            salt TEXT NOT NULL
        )
    )";
    
    QString createUserSessionsTable = R"(
        CREATE TABLE IF NOT EXISTS user_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            session_token TEXT UNIQUE NOT NULL,
            login_time TEXT NOT NULL,
            last_activity TEXT NOT NULL,
            is_active INTEGER NOT NULL DEFAULT 1,
            FOREIGN KEY (username) REFERENCES users (username)
        )
    )";
    
    QString createUserLogsTable = R"(
        CREATE TABLE IF NOT EXISTS user_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            action TEXT NOT NULL,
            details TEXT,
            timestamp TEXT NOT NULL,
            ip_address TEXT,
            FOREIGN KEY (username) REFERENCES users (username)
        )
    )";
    
    QSqlQuery query(db_);
    
    if (!query.exec(createUsersTable)) {
        qDebug() << "创建用户表失败:" << query.lastError().text();
        emit databaseError("创建用户表失败: " + query.lastError().text());
        return false;
    }
    
    if (!query.exec(createUserSessionsTable)) {
        qDebug() << "创建用户会话表失败:" << query.lastError().text();
        emit databaseError("创建用户会话表失败: " + query.lastError().text());
        return false;
    }
    
    if (!query.exec(createUserLogsTable)) {
        qDebug() << "创建用户日志表失败:" << query.lastError().text();
        emit databaseError("创建用户日志表失败: " + query.lastError().text());
        return false;
    }
    
    return true;
}

bool UserDatabase::createDefaultSuperAdmin()
{
    UserInfo superAdmin;
    superAdmin.username = "admin";
    superAdmin.password = "admin123";  // 默认密码
    superAdmin.realName = "系统管理员";
    superAdmin.role = UserRole::SuperAdmin;
    superAdmin.permissions = PermissionGroups::getRolePermissions(UserRole::SuperAdmin);
    superAdmin.createTime = QDateTime::currentDateTime();
    superAdmin.isActive = true;
    superAdmin.createdBy = "system";
    
    return addUser(superAdmin);
}

bool UserDatabase::addUser(const UserInfo& user)
{
    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT INTO users (username, password, real_name, role, permissions, 
                          create_time, is_active, created_by, salt)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    
    QString salt = generateSalt();
    QString hashedPassword = hashPassword(user.password, salt);
    
    query.addBindValue(user.username);
    query.addBindValue(hashedPassword);
    query.addBindValue(user.realName);
    query.addBindValue(static_cast<int>(user.role));
    query.addBindValue(user.permissions);
    query.addBindValue(user.createTime.toString(Qt::ISODate));
    query.addBindValue(user.isActive ? 1 : 0);
    query.addBindValue(user.createdBy);
    query.addBindValue(salt);
    
    if (!query.exec()) {
        qDebug() << "添加用户失败:" << query.lastError().text();
        emit databaseError("添加用户失败: " + query.lastError().text());
        return false;
    }
    
    emit userAdded(user);
    return true;
}

bool UserDatabase::updateUser(const UserInfo& user)
{
    QSqlQuery query(db_);
    query.prepare(R"(
        UPDATE users SET 
            username = ?, real_name = ?, role = ?, permissions = ?, 
            is_active = ?, created_by = ?
        WHERE id = ?
    )");
    
    query.addBindValue(user.username);
    query.addBindValue(user.realName);
    query.addBindValue(static_cast<int>(user.role));
    query.addBindValue(user.permissions);
    query.addBindValue(user.isActive ? 1 : 0);
    query.addBindValue(user.createdBy);
    query.addBindValue(user.id);
    
    if (!query.exec()) {
        qDebug() << "更新用户失败:" << query.lastError().text();
        emit databaseError("更新用户失败: " + query.lastError().text());
        return false;
    }
    
    emit userUpdated(user);
    return true;
}

bool UserDatabase::deleteUser(int userId)
{
    QSqlQuery query(db_);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (!query.exec()) {
        qDebug() << "删除用户失败:" << query.lastError().text();
        emit databaseError("删除用户失败: " + query.lastError().text());
        return false;
    }
    
    emit userDeleted(userId);
    return true;
}

bool UserDatabase::deleteUser(const QString& username)
{
    QSqlQuery query(db_);
    query.prepare("DELETE FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec()) {
        qDebug() << "删除用户失败:" << query.lastError().text();
        emit databaseError("删除用户失败: " + query.lastError().text());
        return false;
    }
    
    emit userDeleted(-1); // 使用-1表示通过用户名删除
    return true;
}

UserInfo UserDatabase::getUserById(int userId)
{
    QSqlQuery query(db_);
    query.prepare("SELECT * FROM users WHERE id = ?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        return userFromQuery(query);
    }
    
    return UserInfo();
}

UserInfo UserDatabase::getUserByUsername(const QString& username)
{
    QSqlQuery query(db_);
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return userFromQuery(query);
    }
    
    return UserInfo();
}

QList<UserInfo> UserDatabase::getAllUsers()
{
    QList<UserInfo> users;
    QSqlQuery query(db_);
    query.prepare("SELECT * FROM users ORDER BY create_time DESC");
    
    if (query.exec()) {
        while (query.next()) {
            users.append(userFromQuery(query));
        }
    }
    
    return users;
}

QList<UserInfo> UserDatabase::getUsersByRole(UserRole role)
{
    QList<UserInfo> users;
    QSqlQuery query(db_);
    query.prepare("SELECT * FROM users WHERE role = ? ORDER BY create_time DESC");
    query.addBindValue(static_cast<int>(role));
    
    if (query.exec()) {
        while (query.next()) {
            users.append(userFromQuery(query));
        }
    }
    
    return users;
}

bool UserDatabase::validateUser(const QString& username, const QString& password)
{
    QSqlQuery query(db_);
    query.prepare("SELECT password, salt, is_active FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        QString storedHash = query.value(0).toString();
        QString salt = query.value(1).toString();
        bool isActive = query.value(2).toBool();
        
        if (!isActive) {
            return false; // 用户已被禁用
        }
        
        QString inputHash = hashPassword(password, salt);
        return inputHash == storedHash;
    }
    
    return false;
}

bool UserDatabase::updateLastLoginTime(const QString& username)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE users SET last_login_time = ? WHERE username = ?");
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(username);
    
    return query.exec();
}

bool UserDatabase::updateUserPermissions(int userId, int permissions)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE users SET permissions = ? WHERE id = ?");
    query.addBindValue(permissions);
    query.addBindValue(userId);
    
    return query.exec();
}

bool UserDatabase::updateUserRole(int userId, UserRole role)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE users SET role = ? WHERE id = ?");
    query.addBindValue(static_cast<int>(role));
    query.addBindValue(userId);
    
    return query.exec();
}

bool UserDatabase::activateUser(int userId)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE users SET is_active = 1 WHERE id = ?");
    query.addBindValue(userId);
    
    return query.exec();
}

bool UserDatabase::deactivateUser(int userId)
{
    QSqlQuery query(db_);
    query.prepare("UPDATE users SET is_active = 0 WHERE id = ?");
    query.addBindValue(userId);
    
    return query.exec();
}

bool UserDatabase::isUserActive(const QString& username)
{
    QSqlQuery query(db_);
    query.prepare("SELECT is_active FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    
    return false;
}

void UserDatabase::closeDatabase()
{
    if (db_.isOpen()) {
        db_.close();
    }
}

UserInfo UserDatabase::userFromQuery(const QSqlQuery& query)
{
    UserInfo user;
    user.id = query.value("id").toInt();
    user.username = query.value("username").toString();
    user.password = query.value("password").toString();
    user.realName = query.value("real_name").toString();
    user.role = static_cast<UserRole>(query.value("role").toInt());
    user.permissions = query.value("permissions").toInt();
    user.createTime = QDateTime::fromString(query.value("create_time").toString(), Qt::ISODate);
    user.lastLoginTime = QDateTime::fromString(query.value("last_login_time").toString(), Qt::ISODate);
    user.isActive = query.value("is_active").toBool();
    user.createdBy = query.value("created_by").toString();
    
    return user;
}

QString UserDatabase::generateSalt()
{
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString salt;
    salt.reserve(16);
    
    for (int i = 0; i < 16; ++i) {
        salt.append(chars.at(QRandomGenerator::global()->bounded(chars.length())));
    }
    
    return salt;
}

QString UserDatabase::hashPassword(const QString& password, const QString& salt)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    hash.addData(salt.toUtf8());
    return hash.result().toHex();
}
