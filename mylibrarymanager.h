#ifndef MY_LIBRARY_MANAGER_H
#define MY_LIBRARY_MANAGER_H

#include "ritemmanager.h"

#include <QDateTime>

/// @todo Use IdNameTableManager for AcquiredFrom
/// @todo Change AcquiredDate from QDateTime to QDate class
/// @todo Consider adding updated_at field to track changes
/// @todo Use GMT+3 time zone for created_at and updated_at fields
/// @todo Null option for acquired_date

struct MyLibraryData {
    int r_item_id; ///< ID of the readable item in the library
    QString acquired_from; ///< Source from which the item was acquired
    QDateTime acquired_date; ///< Date when the item was acquired
    double price; ///< Price of the item, if applicable
    QString shelf_name; ///< Name of the shelf where the item is stored
    QString notes; ///< Additional notes about the item
};

class MyLibraryManager
{
public:
    /**
     * @brief Constructs a MyLibraryManager object and initializes the library table.
     * 
     * @param db_manager Pointer to the DatabaseManager instance.
     * @param shelf_manager Pointer to the IdNameTableManager instance for shelves.
     * @param r_item_manager Pointer to the RItemManager instance.
     */
    MyLibraryManager(DatabaseManager* db_manager, IdNameTableManager* shelf_manager, RItemManager* r_item_manager);

    ~MyLibraryManager(); ///< Destructor

    /**
     * @brief Inserts a new item into the library.
     * 
     * @param item_data The data of the item to insert.
     * @return int The MyLibrary ID of the inserted item, or -1 on failure.
     * @todo Consider changing the name of this function to InsertMyLibraryItem for clarity.
     */
    int InsertRItem(const MyLibraryData& item_data);

private:
    DatabaseManager* database_manager; ///< Pointer to the DatabaseManager instance.
    IdNameTableManager* shelf_manager; ///< Pointer to the IdNameTableManager instance for shelves.
    RItemManager* r_item_manager; ///< Pointer to the RItemManager instance.

    void CreateMyLibraryTable(); ///< Creates the MyLibrary table in the database.
};

#endif // MY_LIBRARY_MANAGER_H
