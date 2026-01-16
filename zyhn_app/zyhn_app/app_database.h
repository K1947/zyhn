#ifndef APP_DATABASE_H
#define APP_DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "zyhn_core.h"

using namespace zyhn_core;
class AppDatabase : public IAppDatabase
{
    Q_OBJECT
public:
    explicit AppDatabase(QObject* parent = nullptr);
    ~AppDatabase();

    bool initialize(const QString& dbPath = "") override;
    void close() override;
    int getDatabaseVersion() override;

    bool saveData(const QString& tableName, int id, const QString& jsonData, 
                  const QMap<QString, QVariant>& indexFields = QMap<QString, QVariant>());
    bool deleteData(const QString& tableName, int id) override;

    QVector<QString> loadAllData(const QString& tableName) override;
    QString findDataById(const QString& tableName, int id) override;
    QVector<QString> queryData(const QString& tableName, const QString& whereClause) override;
    int getNextId(const QString& tableName) override;
    bool createTable(const QString& tableName, const QStringList& indexFields = QStringList()) override;
    bool tableExists(const QString& tableName) override;
    QStringList getAllTables() override;
    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;
    bool clearTable(const QString& tableName) override;
    bool dropTable(const QString& tableName) override;
    bool compactDatabase() override;
    QMap<QString, int> getDatabaseStats() override;

signals:
    void dataChanged(const QString& tableName, int id);
    void databaseError(const QString& error);
    
private:
    bool createSystemTables();
    bool executeQuery(const QString& sql);
    bool upgradeDatabase(int fromVersion, int toVersion);

private:
    QSqlDatabase db_;
    QString connection_name_;
    static const int CURRENT_DB_VERSION = 1;
};

#endif // APP_DATABASE_H

