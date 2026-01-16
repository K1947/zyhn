#include "app_database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QUuid>
#include <QCoreApplication>
#include <QDir>

AppDatabase::AppDatabase(QObject* parent)
{
    connection_name_ = QUuid::createUuid().toString();
}

AppDatabase::~AppDatabase()
{
    close();
}

bool AppDatabase::initialize(const QString& dbPath)
{
    QString path = dbPath;
    if (path.isEmpty()) {
        QDir dir(dbPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    }
    path = dbPath + "/app.db";

    db_ = QSqlDatabase::addDatabase("QSQLITE", connection_name_);
    db_.setDatabaseName(path);

    if (!db_.open()) {
        qDebug() << "Failed to open database:" << db_.lastError().text();
        emit databaseError(db_.lastError().text());
        return false;
    }

    qDebug() << "Database opened:" << path;

    if (!createSystemTables()) {
        return false;
    }

    int currentVersion = getDatabaseVersion();
    if (currentVersion < CURRENT_DB_VERSION) {
        if (!upgradeDatabase(currentVersion, CURRENT_DB_VERSION)) {
            qDebug() << "Failed to upgrade database";
            return false;
        }
    }

    return true;
}

void AppDatabase::close()
{
    if (db_.isOpen()) {
        db_.close();
    }
    QSqlDatabase::removeDatabase(connection_name_);
}

bool AppDatabase::createSystemTables()
{
    QSqlQuery query(db_);

    QString createMetaTable = R"(
        CREATE TABLE IF NOT EXISTS _system_meta (
            key TEXT PRIMARY KEY,
            value TEXT,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createMetaTable)) {
        qDebug() << "Failed to create system meta table:" << query.lastError().text();
        return false;
    }

    query.exec(QString("INSERT OR IGNORE INTO _system_meta (key, value) VALUES ('db_version', '%1')")
                   .arg(CURRENT_DB_VERSION));

    QString createTableRegistry = R"(
        CREATE TABLE IF NOT EXISTS _table_registry (
            table_name TEXT PRIMARY KEY,
            index_fields TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            record_count INTEGER DEFAULT 0
        )
    )";

    if (!query.exec(createTableRegistry)) {
        qDebug() << "Failed to create table registry:" << query.lastError().text();
        return false;
    }

    qDebug() << "System tables created successfully";
    return true;
}

int AppDatabase::getDatabaseVersion()
{
    QSqlQuery query(db_);
    query.prepare("SELECT value FROM _system_meta WHERE key = 'db_version'");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

bool AppDatabase::createTable(const QString& tableName, const QStringList& indexFields)
{
    if (tableExists(tableName)) {
        qDebug() << "Table already exists:" << tableName;
        return true;
    }

    QSqlQuery query(db_);

    QString createTableSql = QString(
        "CREATE TABLE IF NOT EXISTS %1 ("
        "  id INTEGER PRIMARY KEY,"
        "  data_json TEXT NOT NULL,"
    ).arg(tableName);

    if (!indexFields.isEmpty()) {
        for (const QString& field : indexFields) {
            createTableSql += QString("  %1 TEXT,").arg(field);
        }
    }

    createTableSql += 
        "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createTableSql)) {
        qDebug() << "Failed to create table" << tableName << ":" << query.lastError().text();
        emit databaseError(query.lastError().text());
        return false;
    }

    for (const QString& field : indexFields) {
        QString indexName = QString("idx_%1_%2").arg(tableName).arg(field);
        QString createIndexSql = QString("CREATE INDEX IF NOT EXISTS %1 ON %2(%3)")
                                     .arg(indexName).arg(tableName).arg(field);
        query.exec(createIndexSql);
    }

    query.prepare("INSERT INTO _table_registry (table_name, index_fields) VALUES (?, ?)");
    query.addBindValue(tableName);
    query.addBindValue(indexFields.join(","));
    query.exec();

    qDebug() << "Table created:" << tableName << "with index fields:" << indexFields;
    return true;
}

bool AppDatabase::tableExists(const QString& tableName)
{
    QSqlQuery query(db_);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue(tableName);
    return query.exec() && query.next();
}

QStringList AppDatabase::getAllTables()
{
    QStringList tables;
    QSqlQuery query(db_);
    
    if (query.exec("SELECT table_name FROM _table_registry ORDER BY table_name")) {
        while (query.next()) {
            tables.append(query.value(0).toString());
        }
    }
    
    return tables;
}

bool AppDatabase::saveData(const QString& tableName, int id, const QString& jsonData, 
                           const QMap<QString, QVariant>& indexFields)
{
    if (!tableExists(tableName)) {
        QStringList fields = indexFields.keys();
        if (!createTable(tableName, fields)) {
            return false;
        }
    }

    QSqlQuery query(db_);

    query.prepare(QString("SELECT id FROM %1 WHERE id = ?").arg(tableName));
    query.addBindValue(id);
    
    bool exists = query.exec() && query.next();

    if (exists) {
        QString updateSql = QString("UPDATE %1 SET data_json = ?").arg(tableName);
        
        QStringList fieldUpdates;
        for (auto it = indexFields.begin(); it != indexFields.end(); ++it) {
            fieldUpdates.append(QString("%1 = ?").arg(it.key()));
        }
        if (!fieldUpdates.isEmpty()) {
            updateSql += ", " + fieldUpdates.join(", ");
        }
        
        updateSql += ", updated_at = CURRENT_TIMESTAMP WHERE id = ?";
        
        query.prepare(updateSql);
        query.addBindValue(jsonData);
        
        for (const QVariant& value : indexFields.values()) {
            query.addBindValue(value);
        }
        query.addBindValue(id);
    } else {
        QStringList columns = QStringList() << "id" << "data_json";
        QStringList placeholders = QStringList() << "?" << "?";
        
        for (const QString& field : indexFields.keys()) {
            columns.append(field);
            placeholders.append("?");
        }
        
        QString insertSql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                                .arg(tableName)
                                .arg(columns.join(", "))
                                .arg(placeholders.join(", "));
        
        query.prepare(insertSql);
        query.addBindValue(id);
        query.addBindValue(jsonData);
        
        for (const QVariant& value : indexFields.values()) {
            query.addBindValue(value);
        }
    }

    if (!query.exec()) {
        qDebug() << "Failed to save data:" << query.lastError().text();
        emit databaseError(query.lastError().text());
        return false;
    }

    emit dataChanged(tableName, id);
    return true;
}

bool AppDatabase::deleteData(const QString& tableName, int id)
{
    if (!tableExists(tableName)) {
        return false;
    }

    QSqlQuery query(db_);
    query.prepare(QString("DELETE FROM %1 WHERE id = ?").arg(tableName));
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Failed to delete data:" << query.lastError().text();
        emit databaseError(query.lastError().text());
        return false;
    }

    emit dataChanged(tableName, id);
    return true;
}

QVector<QString> AppDatabase::loadAllData(const QString& tableName)
{
    QVector<QString> results;
    
    if (!tableExists(tableName)) {
        return results;
    }

    QSqlQuery query(db_);
    if (query.exec(QString("SELECT data_json FROM %1 ORDER BY id").arg(tableName))) {
        while (query.next()) {
            results.append(query.value(0).toString());
        }
    }

    return results;
}

QString AppDatabase::findDataById(const QString& tableName, int id)
{
    if (!tableExists(tableName)) {
        return QString();
    }

    QSqlQuery query(db_);
    query.prepare(QString("SELECT data_json FROM %1 WHERE id = ?").arg(tableName));
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

QVector<QString> AppDatabase::queryData(const QString& tableName, const QString& whereClause)
{
    QVector<QString> results;
    
    if (!tableExists(tableName)) {
        return results;
    }

    QString sql = QString("SELECT data_json FROM %1").arg(tableName);
    if (!whereClause.isEmpty()) {
        sql += " WHERE " + whereClause;
    }
    sql += " ORDER BY id";

    QSqlQuery query(db_);
    if (query.exec(sql)) {
        while (query.next()) {
            results.append(query.value(0).toString());
        }
    }

    return results;
}

int AppDatabase::getNextId(const QString& tableName)
{
    if (!tableExists(tableName)) {
        return 1;
    }

    QSqlQuery query(db_);
    if (query.exec(QString("SELECT MAX(id) FROM %1").arg(tableName)) && query.next()) {
        return query.value(0).toInt() + 1;
    }

    return 1;
}

bool AppDatabase::beginTransaction()
{
    return db_.transaction();
}

bool AppDatabase::commitTransaction()
{
    return db_.commit();
}

bool AppDatabase::rollbackTransaction()
{
    return db_.rollback();
}

bool AppDatabase::clearTable(const QString& tableName)
{
    if (!tableExists(tableName)) {
        return false;
    }

    QSqlQuery query(db_);
    return query.exec(QString("DELETE FROM %1").arg(tableName));
}

bool AppDatabase::dropTable(const QString& tableName)
{
    if (!tableExists(tableName)) {
        return true;
    }

    QSqlQuery query(db_);
    
    query.exec(QString("DELETE FROM _table_registry WHERE table_name = '%1'").arg(tableName));
    return query.exec(QString("DROP TABLE %1").arg(tableName));
}

bool AppDatabase::compactDatabase()
{
    QSqlQuery query(db_);
    return query.exec("VACUUM");
}

QMap<QString, int> AppDatabase::getDatabaseStats()
{
    QMap<QString, int> stats;
    QSqlQuery query(db_);

    QStringList tables = getAllTables();
    for (const QString& table : tables) {
        if (query.exec(QString("SELECT COUNT(*) FROM %1").arg(table)) && query.next()) {
            stats[table] = query.value(0).toInt();
        }
    }

    return stats;
}

bool AppDatabase::executeQuery(const QString& sql)
{
    QSqlQuery query(db_);
    return query.exec(sql);
}

bool AppDatabase::upgradeDatabase(int fromVersion, int toVersion)
{
    qDebug() << "Upgrading database from version" << fromVersion << "to" << toVersion;

    QSqlQuery query(db_);
    query.prepare("UPDATE _system_meta SET value = ?, updated_at = CURRENT_TIMESTAMP WHERE key = 'db_version'");
    query.addBindValue(toVersion);
    
    if (query.exec()) {
        qDebug() << "Database upgraded successfully";
        return true;
    }

    return false;
}

