#ifndef ID_NAME_TABLE_MANAGER_H
#define ID_NAME_TABLE_MANAGER_H

#include "databasemanager.h"

/**
 * @file idnametablemanager.h
 * @brief Header file for IdNameTableManager class.
 *
 * This class manages tables that store IDs and names, allowing insertion,
 * retrieval, and management of various entities like authors, publishers, etc.
 */

enum class IdNameTable {
    Author, ///< Represents the Author table
    Publisher, ///< Represents the Publisher table
    Language, ///< Represents the Language table
    Country, ///< Represents the Country table
    Genre, ///< Represents the Genre table
    Series ///< Represents the Series table
};

/**
 * @class IdNameTableManager
 * @brief Manages ID-Name tables in the database.
 *
 * This class provides methods to insert names, retrieve IDs by names,
 * retrieve names by IDs, and manage the creation of ID-Name tables.
 */
class IdNameTableManager
{
public:

    /**
     * @brief Constructs an IdNameTableManager object and initializes the tables.
     * 
     * @param db_manager Pointer to the DatabaseManager instance.
     */
    IdNameTableManager(IdNameTable table, DatabaseManager* db_manager);

    /**
     * @brief Destroys the IdNameTableManager object.
     */
    ~IdNameTableManager();

    /**
     * @brief Insert a new name into the specified table
     * 
     * @param name Input name to insert
     * @return int The ID of the inserted name, or -1 on failure
     */
    int Insert(const QString& name);

    /**
     * @brief Get the Id By Name in the specified table
     * 
     * @param name Name to search for
     * @return int The ID of the name, or -1 if not found
     */
    int GetIdByName(const QString& name);

    /**
     * @brief Get the Name By Id in the specified table
     *
     * @param id ID to search for
     * @return QString The name associated with the ID, or an empty string if not found
     */
    QString GetNameById(const int id);

    /**
     * @brief Get all names from the specified table descending by id
     * 
     * @return QStringList List of names in the specified table
     */
    QStringList GetAllNames();

private:
    DatabaseManager* database_manager;  ///< Pointer to the DatabaseManager instance
    IdNameTable table; ///< The table type being managed
    QString table_name; ///< The name of the table in the database

    /**
     * @brief Convert IdNameTable enum to string
     * 
     * @return QString Table name as a string
     */
    const QString IdNameTableString();

    void CreateTable(); ///< Creates the ID-Name table in the database

};

#endif //ID_NAME_TABLE_MANAGER_H
