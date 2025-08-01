#include "databasemanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

DatabaseManager::DatabaseManager()
{
    // Determine the AppData location
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);

    // Create the directory if it doesn't exist
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Full path to the database file
    QString dbFilePath = dir.filePath("reading_tracker.db");

    // Set up the database connection
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFilePath);

    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
    }
    else {
        qDebug() << "Database opened successfully at:" << dbFilePath;
    }
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database closed.";
    }
}

QSqlDatabase& DatabaseManager::GetDatabase()
{
    return db;
}
