#include "editionmanager.h"

EditionManager::EditionManager(DatabaseManager* db_manager,
                               IdNameTableManager* publisher_manager,
                               IdNameTableManager* language_manager,
                               IdNameTableManager* series_manager,
                               BookManager* book_manager)
    : database_manager(db_manager),
      publisher_manager(publisher_manager),
      language_manager(language_manager),
      series_manager(series_manager),
      book_manager(book_manager)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    // Initialize edition-related tables
    CreateEditionTable();
}

EditionManager::~EditionManager()
{
    // Destructor logic if needed
}

int EditionManager::InsertEdition(const EditionData& edition_data)
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    if (edition_data.book_id <= 0) {
        qWarning() << "InsertEdition failed: Book ID must be greater than 0";
        return -1; // Invalid input
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO Edition (book_id, publisher_id, language_id, series_id, page_count, publication_date, isbn, type, cover_image_path) "
                  "VALUES (:book_id, :publisher_id, :language_id, :series_id, :page_count, :publication_date, :isbn, :type, :cover_image_path)");
    
    query.bindValue(":book_id", edition_data.book_id);

    int publisher_id = publisher_manager->GetIdByName(edition_data.publisher);
    if (publisher_id == -1) {
        publisher_id = publisher_manager->Insert(edition_data.publisher);
        if (publisher_id == -1) {
            qCritical() << "Failed to insert publisher:" << edition_data.publisher;
            return -1; // Insertion failed
        }
    }
    query.bindValue(":publisher_id", publisher_id);

    // Handle language_id
    int language_id = -1;
    if (!edition_data.language.trimmed().isEmpty()) {
        language_id = language_manager->GetIdByName(edition_data.language);
        if (language_id == -1) {
            language_id = language_manager->Insert(edition_data.language);
            if (language_id == -1) {
                qCritical() << "Failed to insert language:" << edition_data.language;
                return -1; // Insertion failed
            }
        }
        query.bindValue(":language_id", language_id);
    }
    else {
        query.bindValue(":language_id", QVariant(QVariant::Int)); // Will be NULL in SQL
    }

    // Handle series_id
    int series_id = -1;
    if (!edition_data.series.trimmed().isEmpty()) {
        series_id = series_manager->GetIdByName(edition_data.series);
        if (series_id == -1) {
            series_id = series_manager->Insert(edition_data.series);
            if (series_id == -1) {
                qCritical() << "Failed to insert series:" << edition_data.series;
                return -1; // Insertion failed
            }
        }
        query.bindValue(":series_id", series_id);
    }
    else {
        query.bindValue(":series_id", QVariant(QVariant::Int)); // Will be NULL in SQL
    }

    if(edition_data.page_count <= 0) {
        query.bindValue(":page_count", QVariant(QVariant::Int)); // Will be NULL in SQL
    }
    else {
        query.bindValue(":page_count", edition_data.page_count);
    }
    query.bindValue(":publication_date", edition_data.publication_date);
    query.bindValue(":isbn", edition_data.isbn);
    query.bindValue(":type", edition_data.type);
    query.bindValue(":cover_image_path", edition_data.cover_image_path);

    if (!query.exec()) {
        qCritical() << "InsertEdition:" << query.lastError().text();
        return -1; // Insertion failed
    }

    return query.lastInsertId().toInt(); // Return the ID of the inserted edition
}

QMap<int, QString> EditionManager::GetAllEditions() const
{
    QMap<int, QString> editions;

    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return editions;
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Get all editions with their IDs, book_id, and publisher_id
    if (!query.exec("SELECT Edition.id, Edition.book_id, Publisher.name "
                    "FROM Edition "
                    "LEFT JOIN Publisher ON Edition.publisher_id = Publisher.id")) {
        qCritical() << "GetAllEditions:" << query.lastError().text();
        return editions;
    }

    while (query.next()) {
        int edition_id = query.value(0).toInt();
        int book_id = query.value(1).toInt();
        QString publisher = query.value(2).toString();

        // Get book title and authors using BookManager
        QString label;
        if (book_manager) {
            // Get title
            QSqlQuery bookQuery(db);
            bookQuery.prepare("SELECT title FROM Book WHERE id = :book_id");
            bookQuery.bindValue(":book_id", book_id);
            QString title;
            if (bookQuery.exec() && bookQuery.next()) {
                title = bookQuery.value(0).toString();
            }

            // Get authors
            QStringList authors = book_manager->GetAuthorsForBook(book_id);

            label = title;
            if (!publisher.isEmpty()) {
                label += " - " + publisher;
            }
            if (!authors.isEmpty()) {
                label += " - " + authors.join(", ");
            }
        }
        else {
            label = QString("BookID %1").arg(book_id);
            if (!publisher.isEmpty()) {
                label += " - " + publisher;
            }
        }

        editions.insert(edition_id, label);
    }

    return editions;
}

void EditionManager::CreateEditionTable()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS Edition ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "book_id INTEGER NOT NULL, "
               "publisher_id INTEGER NOT NULL, "
               "language_id INTEGER, "
               "series_id INTEGER, "
               "page_count INTEGER, "
               "publication_date TEXT, "
               "isbn TEXT, "
               "type TEXT, "
               "cover_image_path TEXT, "
               "FOREIGN KEY(book_id) REFERENCES Book(id), "
               "FOREIGN KEY(publisher_id) REFERENCES Publisher(id), "
               "FOREIGN KEY(language_id) REFERENCES Language(id), "
               "FOREIGN KEY(series_id) REFERENCES Series(id))");
}