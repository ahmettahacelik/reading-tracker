#ifndef R_ITEM_MANAGER_H
#define R_ITEM_MANAGER_H

#include "editionmanager.h"

enum class RItemType {
    Edition,    ///< Represents an edition item
    Issue       ///< Represents an issue item
};

struct RItemData {
    int edition_id; ///< ID of the edition this item belongs to
    int issue_id; ///< ID of the issue this item belongs to
    RItemType type; ///< Type of the item (Edition or Issue)
};

class RItemManager
{
public:
    /**
     * @brief Constructs an RItemManager object.
     * 
     * @param db_manager Pointer to the DatabaseManager instance.
     * @param edition_manager Pointer to the EditionManager instance.
     */
    RItemManager(DatabaseManager* db_manager, EditionManager* edition_manager);

    ~RItemManager(); ///< Destructor

    /**
     * @brief Inserts a new edition into the readable item table.
     * 
     * @param edition_data The edition data of the item to insert.
     * @return int r_item_id, The ID of the inserted item, or -1 on failure.
     */
    int InsertEdition(const EditionData& edition_data);

    /**
     * @brief Checks if a readable item exists in the database.
     * 
     * @param r_item_id The ID of the readable item to check.
     * @return true if the item exists, false otherwise.
     */
    bool RItemExists(int r_item_id) const;

    /**
     * @brief Retrieves all readable items from the database.
     * 
     * @return QMap<int, QString> A map of readable item IDs to their labels.
     */
    QMap<int, QString> GetAllRItems() const;

    /// @todo IssueManager should be implemented similarly to EditionManager
    // int InsertIssue(const IssueData& issue_data);

private:
    DatabaseManager* database_manager; ///< Pointer to the DatabaseManager instance.
    EditionManager* edition_manager; ///< Pointer to the EditionManager instance.

    void CreateRItemTable(); ///< Creates the RItem table in the database.

    int InsertRItem(const RItemData& item_data); ///< Inserts a new RItem into the database.
};

#endif // R_ITEM_MANAGER_H