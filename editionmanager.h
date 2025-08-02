#ifndef EDITION_MANAGER_H
#define EDITION_MANAGER_H

#include "bookmanager.h"

struct EditionData {
    int id; ///< Unique identifier for the edition
    int book_id; ///< ID of the book this edition belongs to
    QString publisher; ///< Publisher of the edition
    QString language; ///< Language of the edition
    QString series; ///< Series this edition belongs to
    int page_count; ///< Number of pages in the edition
    QString publication_date; ///< Publication date of the edition
    QString isbn; ///< ISBN number of the edition
    QString type; ///< Type of the edition (e.g., hardcover, paperback)
    QString cover_image_path; ///< Path to the cover image of the edition
};

class EditionManager
{
public:
    /**
     * @brief Constructs an EditionManager object.
     * 
     * @param db_manager Pointer to the DatabaseManager instance.
     * @param id_name_table_manager Pointer to the IdNameTableManager instance.
     * @param book_manager Pointer to the BookManager instance.
     */
    EditionManager(DatabaseManager* db_manager, IdNameTableManager* id_name_table_manager, BookManager* book_manager);

    /**
     * @brief Destroys the EditionManager object.
     */
    ~EditionManager();

    /**
     * @brief Inserts a new edition into the database.
     * 
     * @param edition_data The data of the edition to insert.
     * @return int The ID of the inserted edition, or -1 on failure.
     */
    int InsertEdition(const EditionData& edition_data);

private:
    DatabaseManager* database_manager; ///< Pointer to the DatabaseManager instance.
    IdNameTableManager* id_name_table_manager; ///< Pointer to the IdNameTableManager instance.
    BookManager* book_manager; ///< Pointer to the BookManager instance.

    void CreateEditionTable(); ///< Creates the edition table in the database.
};

#endif // EDITION_MANAGER_H