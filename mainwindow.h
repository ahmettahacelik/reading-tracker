#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editionmanager.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonAddBook_clicked();
    void on_pushButtonAddEdition_clicked();

private:
    Ui::MainWindow *ui;
    EditionManager* edition_manager; ///< Pointer to the EditionManager instance.
    BookManager* book_manager; ///< Pointer to the BookManager instance.
    IdNameTableManager* id_name_table_manager; ///< Pointer to the IdNameTableManager instance.
    DatabaseManager* database_manager; ///< Pointer to the DatabaseManager instance.

    void RefreshBookCompleters(); ///< Refreshes the completers for input fields.

    void RefreshEditionCompleters(); ///< Refreshes the completers for edition-related input fields.

    void RefreshEditionsView(); ///< Refreshes the editions view in the UI.
};
#endif // MAINWINDOW_H
