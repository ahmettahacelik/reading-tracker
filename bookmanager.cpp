#include "bookmanager.h"

BookManager::BookManager(DatabaseManager* db_manager,
                         IdNameTableManager* author_manager,
                         IdNameTableManager* language_manager,
                         IdNameTableManager* country_manager,
                         IdNameTableManager* genre_manager)
    : database_manager(db_manager),
      author_manager(author_manager),
      language_manager(language_manager),
      country_manager(country_manager),
      genre_manager(genre_manager)
{
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    // Initialize book-related tables
    CreateBookTable();
    CreateBook2AuthorTable();
    CreateBook2GenreTable();
}

BookManager::~BookManager()
{
    // Destructor logic if needed
}

int BookManager::InsertBook(const BookData& book_data)
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return -1; // Database error
    }

    if(!author_manager || !language_manager || !country_manager || !genre_manager) {
        qCritical() << "IdNameTableManager instances are not initialized.";
        return -1; // Initialization error
    }

    if(book_data.title.isEmpty()) {
        qWarning() << "InsertBook failed: Title cannot be empty";
        return -1; // Invalid input
    }

    if(book_data.authors.isEmpty()) {
        qWarning() << "InsertBook failed: At least one author must be provided";
        return -1; // Invalid input
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("INSERT INTO Book (title, org_lang_id, country_id, type) VALUES (:title, :org_lang_id, :country_id, :type)");

    query.bindValue(":title", book_data.title);

    // Handle original language (nullable)
    if (!book_data.original_language.trimmed().isEmpty()) {
        int org_lang_id = language_manager->GetIdByName(book_data.original_language);
        if(org_lang_id == -1) {
            org_lang_id = language_manager->Insert(book_data.original_language);
            if(org_lang_id == -1) {
                qCritical() << "Failed to insert original language:" << book_data.original_language;
                return -1; // Insertion failed
            }
        }
        query.bindValue(":org_lang_id", org_lang_id);
    } else {
        query.bindValue(":org_lang_id", QVariant(QVariant::Int)); // NULL
    }

    // Handle country (nullable)
    if (!book_data.country.trimmed().isEmpty()) {
        int country_id = country_manager->GetIdByName(book_data.country);
        if(country_id == -1) {
            country_id = country_manager->Insert(book_data.country);
            if(country_id == -1) {
                qCritical() << "Failed to insert country:" << book_data.country;
                return -1; // Insertion failed
            }
        }
        query.bindValue(":country_id", country_id);
    } else {
        query.bindValue(":country_id", QVariant(QVariant::Int)); // NULL
    }

    // Handle type (nullable)
    if (!book_data.type.trimmed().isEmpty()) {
        query.bindValue(":type", book_data.type);
    } else {
        query.bindValue(":type", QVariant(QVariant::String)); // NULL
    }

    if (!query.exec()) {
        qCritical() << "InsertBook:" << query.lastError().text();
        return -1; // Insertion failed
    }

    int book_id = query.lastInsertId().toInt();

    // Insert authors
    for (const QString& author : book_data.authors) {
        int author_id = author_manager->GetIdByName(author);
        if (author_id == -1) {
            author_id = author_manager->Insert(author);
            if (author_id == -1) {
                qCritical() << "Failed to insert author:" << author;
                return -1; // Insertion failed
            }
        }
        query.prepare("INSERT INTO Book2Author (book_id, author_id) VALUES (:book_id, :author_id)");
        query.bindValue(":book_id", book_id);
        query.bindValue(":author_id", author_id);
        if (!query.exec()) {
            qCritical() << "InsertBook2Author:" << query.lastError().text();
            /// @todo Handle insertion failure for authors
        }
    }

    // Insert genres (optional)
    for (const QString& genre : book_data.genres) {
        if (genre.trimmed().isEmpty())
            continue;
        int genre_id = genre_manager->GetIdByName(genre);
        if (genre_id == -1) {
            genre_id = genre_manager->Insert(genre);
            if (genre_id == -1) {
                qCritical() << "Failed to insert genre:" << genre;
                return -1; // Insertion failed
            }
        }
        query.prepare("INSERT INTO Book2Genre (book_id, genre_id) VALUES (:book_id, :genre_id)");
        query.bindValue(":book_id", book_id);
        query.bindValue(":genre_id", genre_id);
        if (!query.exec()) {
            qCritical() << "InsertBook2Genre:" << query.lastError().text();
            /// @todo Handle insertion failure for genres
        }
    }

    return book_id; // Return the ID of the inserted book
}

QMap<int, QString> BookManager::GetAllBooks() const
{
    QMap<int, QString> books;

    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return books; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    // Get all books with their IDs and titles
    if (!query.exec("SELECT id, title FROM Book ORDER BY title")) {
        qCritical() << "GetAllBooks:" << query.lastError().text();
        return books;
    }

    while (query.next()) {
        int book_id = query.value(0).toInt();
        QString title = query.value(1).toString();
        QStringList authors = GetAuthorsForBook(book_id);
        QString display = title;
        if (!authors.isEmpty()) {
            display += " - " + authors.join(", ");
        }
        books.insert(book_id, display);
    }

    return books;
}

// Helper function to get authors for a specific book
QStringList BookManager::GetAuthorsForBook(int book_id) const
{
    QStringList authors;

    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return authors;
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT Author.name FROM Author "
                  "INNER JOIN Book2Author ON Author.id = Book2Author.author_id "
                  "WHERE Book2Author.book_id = :book_id "
                  "ORDER BY Author.name");
    query.bindValue(":book_id", book_id);

    if (!query.exec()) {
        qCritical() << "GetAuthorsForBook:" << query.lastError().text();
        return authors;
    }

    while (query.next()) {
        authors.append(query.value(0).toString());
    }

    return authors;
}

void BookManager::CreateBookTable()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS Book ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT NOT NULL, "
               "org_lang_id INTEGER, "
               "country_id INTEGER, "
               "type TEXT, "
               "FOREIGN KEY(org_lang_id) REFERENCES Language(id), "
               "FOREIGN KEY(country_id) REFERENCES Country(id))");
}

void BookManager::CreateBook2AuthorTable()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS Book2Author ("
               "book_id INTEGER, "
               "author_id INTEGER, "
               "PRIMARY KEY(book_id, author_id), "
               "FOREIGN KEY(book_id) REFERENCES Book(id), "
               "FOREIGN KEY(author_id) REFERENCES Author(id))");
}

void BookManager::CreateBook2GenreTable()
{
    // Ensure the database connection is valid
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }

    QSqlDatabase db = database_manager->GetDatabase();
    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS Book2Genre ("
               "book_id INTEGER, "
               "genre_id INTEGER, "
               "PRIMARY KEY(book_id, genre_id), "
               "FOREIGN KEY(book_id) REFERENCES Book(id), "
               "FOREIGN KEY(genre_id) REFERENCES Genre(id))");
}