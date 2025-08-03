#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include "idnametablemanager.h"

struct BookData {
    QString title; ///< Title of the book
    QStringList authors; ///< List of authors of the book
    QStringList genres; ///< List of genres of the book
    QString original_language; ///< Original language of the book
    QString country; ///< Country of publication
    QString type; ///< Type of the book (I am not sure what this means)
};

/**
 * @brief BookManager class
 * This class manages book-related operations such as inserting books and retrieving book information.
 * It uses IdNameTableManager to manage related entities like authors and genres.
 * 
 */
class BookManager
{
public:
    /**
     * @brief Constructs a BookManager object and initializes the book-related tables.
     * 
     * @param db_manager Pointer to the DatabaseManager instance.
     * @param author_manager Pointer to the IdNameTableManager instance for Author table.
     * @param language_manager Pointer to the IdNameTableManager instance for Language table.
     * @param country_manager Pointer to the IdNameTableManager instance for Country table.
     * @param genre_manager Pointer to the IdNameTableManager instance for Genre table.
     */
    BookManager(DatabaseManager* db_manager,
                IdNameTableManager* author_manager,
                IdNameTableManager* language_manager,
                IdNameTableManager* country_manager,
                IdNameTableManager* genre_manager);

    /**
     * @brief Destroys the BookManager object.
     */
    ~BookManager();

    /**
     * @brief Inserts a new book into the database.
     * 
     * @param book_data The data of the book to insert.
     * @return int The ID of the inserted book, or -1 on failure.
     */
    int InsertBook(const BookData& book_data);


    /**
     * @brief Get the All Books in the database.
     * 
     * @return QMap<int, QString> A map of book IDs to their titles, with authors included in the title.
     */
    QMap<int, QString> GetAllBooks() const;

    /**
     * @brief Get the Authors For Book
     * 
     * @param book_id The ID of the book to retrieve authors for.
     * @return QStringList List of authors for the specified book.
     */
    QStringList GetAuthorsForBook(int book_id) const;

private:
    DatabaseManager* database_manager; ///< Pointer to the DatabaseManager instance.
    IdNameTableManager* author_manager; ///< Pointer to the IdNameTableManager instance for authors.
    IdNameTableManager* language_manager; ///< Pointer to the IdNameTableManager instance for languages.
    IdNameTableManager* country_manager; ///< Pointer to the IdNameTableManager instance for countries.
    IdNameTableManager* genre_manager; ///< Pointer to the IdNameTableManager instance for genres.

    void CreateBookTable(); ///< Creates the book table in the database.

    /// @todo Consider creating a junction class for Book2Author and Book2Genre to manage many-to-many relationships.
    void CreateBook2AuthorTable(); ///< Creates the book-author association table in the database.

    void CreateBook2GenreTable(); ///< Creates the book-genre association table in the database.
};

#endif // BOOK_MANAGER_H
