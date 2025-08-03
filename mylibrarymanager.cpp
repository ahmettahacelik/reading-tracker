#include "mylibrarymanager.h"

MyLibraryManager::MyLibraryManager(DatabaseManager* db_manager, IdNameTableManager* shelf_manager, RItemManager* r_item_manager)
    : database_manager(db_manager), shelf_manager(shelf_manager), r_item_manager(r_item_manager)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    CreateMyLibraryTable();
}

MyLibraryManager::~MyLibraryManager()
{
    // Cleanup if necessary
}

int MyLibraryManager::InsertRItem(const MyLibraryData& item_data)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    // Ensure r_item_id is valid and in the RItem table
    if (!r_item_manager->RItemExists(item_data.r_item_id)) {
        qWarning() << "Invalid r_item_id provided.";
        return -1; // Invalid data
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO MyLibrary (r_item_id, acquired_from, acquired_date, price, shelf_id, notes) "
                  "VALUES (:r_item_id, :acquired_from, :acquired_date, :price, :shelf_id, :notes)");
    query.bindValue(":r_item_id", item_data.r_item_id);
    query.bindValue(":acquired_from", item_data.acquired_from.trimmed().isEmpty() ? QVariant(QVariant::String) : item_data.acquired_from);
    query.bindValue(":acquired_date", item_data.acquired_date.isNull() ? QVariant(QVariant::DateTime) : item_data.acquired_date);
    query.bindValue(":price", (item_data.price == 0.0) ? QVariant(QVariant::Double) : item_data.price);
    int shelf_id = shelf_manager->InsertIfNotExists(item_data.shelf_name);
    if(shelf_id == -1) {
        query.bindValue(":shelf_id", QVariant(QVariant::Int)); // Will be NULL in SQL
    }
    else {
        query.bindValue(":shelf_id", shelf_id);
    }
    query.bindValue(":notes", item_data.notes.trimmed().isEmpty() ? QVariant(QVariant::String) : item_data.notes);

    if (!query.exec()) {
        qCritical() << "Failed to insert MyLibrary data:" << query.lastError().text();
        return -1; // Insertion failed
    }

    return query.lastInsertId().toInt(); // Return the MyLibrary ID of the inserted item
}

void MyLibraryManager::CreateMyLibraryTable()
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Create the MyLibrary table if it does not exist
    query.exec("CREATE TABLE IF NOT EXISTS MyLibrary ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "r_item_id INTEGER NOT NULL, "
               "acquired_from TEXT, "                // Or INTEGER if referencing AcquiredFrom table
               "acquired_date DATETIME, "
               "price FLOAT, "
               "shelf_id INTEGER, "
               "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "notes TEXT, "
               "FOREIGN KEY(r_item_id) REFERENCES RItem(id), "
               "FOREIGN KEY(shelf_id) REFERENCES Shelf(id)"
               // If acquired_from is an id, add: FOREIGN KEY(acquired_from)
               ")");
}
