#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editionmanager.h"

#include <QMainWindow>
#include <QLineEdit>

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
    IdNameTableManager* author_manager; ///< Pointer to the IdNameTableManager instance for authors.
    IdNameTableManager* language_manager; ///< Pointer to the IdNameTableManager instance for
    IdNameTableManager* country_manager; ///< Pointer to the IdNameTableManager instance for countries.
    IdNameTableManager* genre_manager; ///< Pointer to the IdNameTableManager instance for
    BookManager* book_manager; ///< Pointer to the BookManager instance.
    IdNameTableManager* publisher_manager; ///< Pointer to the IdNameTableManager instance for publishers.
    IdNameTableManager* series_manager; ///< Pointer to the IdNameTableManager instance for series.
    EditionManager* edition_manager; ///< Pointer to the EditionManager instance.

    void RefreshBookCompleters(); ///< Refreshes the completers for input fields.

    void RefreshEditionCompleters(); ///< Refreshes the completers for edition-related input fields.

    void RefreshQCompleter(IdNameTableManager* manager, QLineEdit* lineEdit); ///< Refreshes a specific completer for a given IdNameTableManager and QLineEdit.

    void RefreshEditionsView(); ///< Refreshes the editions view in the UI.
};
#endif // MAINWINDOW_H
