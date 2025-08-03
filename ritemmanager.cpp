#include "ritemmanager.h"

RItemManager::RItemManager(DatabaseManager* db_manager, EditionManager* edition_manager)
    : database_manager(db_manager), edition_manager(edition_manager)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    // Initialize RItem-related tables
    CreateRItemTable();
}

RItemManager::~RItemManager()
{
    // Cleanup if necessary
}

int RItemManager::InsertEdition(const EditionData& edition_data)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    // Insert the edition data into the database
    int edition_id = edition_manager->InsertEdition(edition_data);
    if (edition_id == -1) {
        qCritical() << "Failed to insert edition data.";
        return -1; // Insertion failed
    }

    RItemData item_data;
    item_data.type = RItemType::Edition;
    item_data.edition_id = edition_id;
    item_data.issue_id = -1; // NULL

    int r_item_id = InsertRItem(item_data);

    return r_item_id;
}

bool RItemManager::RItemExists(int r_item_id) const
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return false;
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT 1 FROM RItem WHERE id = :id LIMIT 1");
    query.bindValue(":id", r_item_id);

    if (!query.exec()) {
        qCritical() << "RItemExists:" << query.lastError().text();
        return false;
    }

    return query.next(); // Returns true if a row exists
}

/// @todo Return with more details about the item, not just IDs
QMap<int, QString> RItemManager::GetAllRItems() const
{
    QMap<int, QString> r_items;

    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return r_items;
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Get all readable items with their IDs and types
    if (!query.exec("SELECT id, type FROM RItem")) {
        qCritical() << "GetAllRItems:" << query.lastError().text();
        return r_items;
    }

    while (query.next()) {
        int r_item_id = query.value(0).toInt();
        RItemType type = static_cast<RItemType>(query.value(1).toInt());

        QString label;
        if (type == RItemType::Edition) {
            label = QString("Edition ID %1").arg(r_item_id);
        }
        else if (type == RItemType::Issue) {
            label = QString("Issue ID %1").arg(r_item_id);
        }
        else {
            label = QString("Unknown Type ID %1").arg(r_item_id);
        }

        r_items.insert(r_item_id, label);
    }

    return r_items;
}

void RItemManager::CreateRItemTable()
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Create the RItem table if it does not exist
    query.exec("CREATE TABLE IF NOT EXISTS RItem ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "type INTEGER, " // Store enum as INTEGER, 0 for edition, 1 for issue
               "edition_id INTEGER, "
               "issue_id INTEGER, "
               "FOREIGN KEY(edition_id) REFERENCES Edition(id), "
               "FOREIGN KEY(issue_id) REFERENCES Issue(id))");
}

int RItemManager::InsertRItem(const RItemData& item_data)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO RItem (type, edition_id, issue_id) VALUES (:type, :edition_id, :issue_id)");
    query.bindValue(":type", static_cast<int>(item_data.type));

    // Only one of edition_id or issue_id should be set, the other should be NULL
    if(item_data.type == RItemType::Edition && item_data.edition_id > 0 && item_data.issue_id == -1) {
        query.bindValue(":edition_id", item_data.edition_id);
        query.bindValue(":issue_id", QVariant(QVariant::Int)); // Will be NULL in SQL
    }
    else if(item_data.type == RItemType::Issue && item_data.issue_id > 0 && item_data.edition_id == -1) {
        query.bindValue(":edition_id", QVariant(QVariant::Int)); // Will be NULL in SQL
        query.bindValue(":issue_id", item_data.issue_id);
    }
    else {
        qWarning() << "InsertRItem failed: Invalid item data";
        return -1; // Invalid input
    }

    if (!query.exec()) {
        qCritical() << "InsertRItem:" << query.lastError().text();
        return -1; // Insertion failed
    }

    return query.lastInsertId().toInt(); // Return the ID of the inserted RItem
}
