#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

/**
 * @brief 
 * 
 */
class DatabaseManager
{
public:
    /**
     * @brief Constructs a DatabaseManager object and initializes the database connection.
     */
    DatabaseManager();

    /**
     * @brief Closes the database connection and cleans up resources.
     */
    ~DatabaseManager();

    /**
     * @brief Returns a reference to the underlying QSqlDatabase instance.
     * @return QSqlDatabase& Reference to the connected database object.
     */
    QSqlDatabase& GetDatabase();

private:
    QSqlDatabase db; ///< The database connection object
};

#endif // DATABASE_MANAGER_H
