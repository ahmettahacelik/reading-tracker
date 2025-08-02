#include "idnametablemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

IdNameTableManager::IdNameTableManager(DatabaseManager* db_manager)
: database_manager(db_manager)
{
    // Initialize tables
    CreateTable(IdNameTable::Author);
    CreateTable(IdNameTable::Publisher);
    CreateTable(IdNameTable::Language);
    CreateTable(IdNameTable::Country);
    CreateTable(IdNameTable::Genre);
    CreateTable(IdNameTable::Series);
}

IdNameTableManager::~IdNameTableManager()
{
    // Destructor logic if needed
}

int IdNameTableManager::Insert(IdNameTable table, const QString& name)
{
    if (name.isEmpty()) {
        qWarning() << "Insert failed: name cannot be empty";
        return -1; // Invalid input
    }

    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    // Prepare the query to insert the name
    // Use INSERT OR IGNORE to avoid duplicates
    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Convert IdNameTable enum to string for the table name
    QString tableName = IdNameTableString(table);

    // Try inserting only if not exists
    query.prepare(QString("INSERT OR IGNORE INTO %1 (name) VALUES (:name)").arg(tableName));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qCritical() << "Insert into" << tableName << ":" << query.lastError().text();
        return -1;
    }

    // Fetch ID
    return GetIdByName(table, name);
}

int IdNameTableManager::GetIdByName(IdNameTable table, const QString& name)
{
    if (name.isEmpty()) {
        qWarning() << "Insert failed: name cannot be empty";
        return -1; // Invalid input
    }

    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);
    QString tableName = IdNameTableString(table);

    query.prepare(QString("SELECT id FROM %1 WHERE name = :name").arg(tableName));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qCritical() << "GetIdByName from" << tableName << ":" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return -1; // Not found
}

QString IdNameTableManager::GetNameById(IdNameTable table, int id)
{
    if(id <= 0) {
        qWarning() << "GetNameById failed: ID must be greater than 0";
        return {}; // Invalid input
    }

    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return {}; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);
    QString tableName = IdNameTableString(table);

    query.prepare(QString("SELECT name FROM %1 WHERE id = :id").arg(tableName));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qCritical() << "GetNameById from" << tableName << ":" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return {};
}

QStringList IdNameTableManager::GetAllNames(IdNameTable table)
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return {}; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);
    QStringList names;
    QString tableName = IdNameTableString(table);

    if (!query.exec(QString("SELECT name FROM %1 ORDER BY name").arg(tableName))) {
        qCritical() << "GetAllNames from" << tableName << ":" << query.lastError().text();
        return names;
    }

    while (query.next()) {
        names << query.value(0).toString();
    }

    return names;
}

const QString IdNameTableManager::IdNameTableString(IdNameTable table)
{
    switch (table) {
        case IdNameTable::Author: return "Author";
        case IdNameTable::Publisher: return "Publisher";
        case IdNameTable::Language: return "Language";
        case IdNameTable::Country: return "Country";
        case IdNameTable::Genre: return "Genre";
        case IdNameTable::Series: return "Series";
        default: return "";
    }
}

void IdNameTableManager::CreateTable(IdNameTable table)
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }
    
    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);
    QString tableName = IdNameTableString(table);

    if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1 ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "name TEXT UNIQUE NOT NULL)").arg(tableName))) {
        qCritical() << "Create" << tableName << ":" << query.lastError().text();
    }
}
