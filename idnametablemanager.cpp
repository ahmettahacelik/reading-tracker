#include "idnametablemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

IdNameTableManager::IdNameTableManager(DatabaseManager* db_manager, IdNameTable table)
    : database_manager(db_manager),
      table(table),
      table_name(IdNameTableString())
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }
    CreateTable(); // Create the table if it doesn't exist
}

IdNameTableManager::~IdNameTableManager()
{
    // Destructor logic if needed
}

int IdNameTableManager::Insert(const QString& name)
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

    // Try inserting only if not exists
    query.prepare(QString("INSERT OR IGNORE INTO %1 (name) VALUES (:name)").arg(table_name));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qCritical() << "Insert into" << table_name << ":" << query.lastError().text();
        return -1;
    }

    // Fetch ID
    return GetIdByName(name);
}

int IdNameTableManager::GetIdByName(const QString& name)
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

    query.prepare(QString("SELECT id FROM %1 WHERE name = :name").arg(table_name));
    query.bindValue(":name", name);
    if (!query.exec()) {
        qCritical() << "GetIdByName from" << table_name << ":" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return -1; // Not found
}

QString IdNameTableManager::GetNameById(int id)
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

    query.prepare(QString("SELECT name FROM %1 WHERE id = :id").arg(table_name));
    query.bindValue(":id", id);
    if (!query.exec()) {
        qCritical() << "GetNameById from" << table_name << ":" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return {};
}

QStringList IdNameTableManager::GetAllNames()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return {}; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);
    QStringList names;

    if (!query.exec(QString("SELECT name FROM %1 ORDER BY name").arg(table_name))) {
        qCritical() << "GetAllNames from" << table_name << ":" << query.lastError().text();
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

void IdNameTableManager::CreateTable()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }
    
    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1 ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "name TEXT UNIQUE NOT NULL)").arg(table_name))) {
        qCritical() << "Create" << table_name << ":" << query.lastError().text();
    }
}
