#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCompleter>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize EditionManager and BookManager
    DatabaseManager* database_manager = new DatabaseManager();

    author_manager = new IdNameTableManager(database_manager, IdNameTable::Author);
    language_manager = new IdNameTableManager(database_manager, IdNameTable::Language);
    country_manager = new IdNameTableManager(database_manager, IdNameTable::Country);
    genre_manager = new IdNameTableManager(database_manager, IdNameTable::Genre);
    book_manager = new BookManager(database_manager, author_manager, language_manager, country_manager, genre_manager);

    publisher_manager = new IdNameTableManager(database_manager, IdNameTable::Publisher);
    //language_manager = new IdNameTableManager(database_manager, IdNameTable::Language); // Already created for BookManager
    series_manager = new IdNameTableManager(database_manager, IdNameTable::Series);
    edition_manager = new EditionManager(database_manager, publisher_manager, language_manager, series_manager, book_manager);

    r_item_manager = new RItemManager(database_manager, edition_manager);

    shelf_manager = new IdNameTableManager(database_manager, IdNameTable::Shelf);
    my_library_manager = new MyLibraryManager(database_manager, shelf_manager, r_item_manager);

    // Set up completers for input fields
    RefreshBookCompleters();

    RefreshEditionCompleters();
    RefreshEditionsView();

    RefreshMyLibraryCompleters();
}

MainWindow::~MainWindow()
{
    delete my_library_manager;
    delete shelf_manager;
    delete r_item_manager;
    delete edition_manager;
    delete series_manager;
    delete publisher_manager;
    delete book_manager;
    delete genre_manager;
    delete country_manager;
    delete language_manager;
    delete author_manager;
    delete ui;
}

void MainWindow::on_pushButtonAddBook_clicked()
{
    BookData book_data;

    // Collect book data from UI inputs (not shown here)
    book_data.title = ui->lineEditTitle->text();
    book_data.authors = ui->lineEditAuthors->text().split(", ");
    QString original_language = ui->lineEditOriginalLanguage->text();
    if (!original_language.isEmpty()) {
        book_data.original_language = original_language;
    }
    QString country = ui->lineEditCountry->text();
    if (!country.isEmpty()) {
        book_data.country = country;
    }
    QStringList genres = ui->lineEditGenres->text().split(", ");
    if (!genres.isEmpty()) {
        book_data.genres = genres;
    }

    // Validate book data
    if(book_data.title.isEmpty() || book_data.authors.isEmpty()) {
        qWarning() << "Book title and authors cannot be empty.";
        return;
    }

    int book_id = book_manager->InsertBook(book_data);

    if (book_id != -1) {
        QMessageBox::information(this, "Success", "Book added successfully!");
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to add book.");
    }

    // Clear input fields after adding the book
    ui->lineEditTitle->clear();
    ui->lineEditAuthors->clear();
    ui->lineEditOriginalLanguage->clear();
    ui->lineEditCountry->clear();
    ui->lineEditGenres->clear();

    RefreshBookCompleters(); // Refresh completers to include new entries
    ui->lineEditTitle->setFocus(); // Set focus back to title input

    RefreshEditionCompleters();
}

void MainWindow::RefreshBookCompleters()
{
    // Refresh completers for input fields
    RefreshQCompleter(author_manager, ui->lineEditAuthors);
    RefreshQCompleter(language_manager, ui->lineEditOriginalLanguage);
    RefreshQCompleter(country_manager, ui->lineEditCountry);
    RefreshQCompleter(genre_manager, ui->lineEditGenres);
}

/// @todo Rename to AddRItem instead of AddEdition
/// @todo Update UI to reflect that this adds an RItem, not just an edition
/// @bug When page_count is not set, it inserts last page_count value instead of NULL
void MainWindow::on_pushButtonAddEdition_clicked()
{
    EditionData edition_data;

    edition_data.book_id = ui->comboBoxBook->currentData().toInt();
    edition_data.publisher = ui->lineEditPublisher->text();
    QString language = ui->lineEditLanguage->text();
    if (!language.isEmpty()) {
        edition_data.language = language;
    }
    QString series = ui->lineEditSeries->text();
    if (!series.isEmpty()) {
        edition_data.series = series;
    }
    int page_count = ui->spinBoxPageCount->value();
    if (page_count > 0) {
        edition_data.page_count = page_count;
    }

    if(edition_data.book_id <= 0 || edition_data.publisher.isEmpty()) {
        qWarning() << "Book ID and publisher cannot be empty.";
        return;
    }

    int r_item_id = r_item_manager->InsertEdition(edition_data);

    if (r_item_id != -1) {
        QMessageBox::information(this, "Success", "Edition added successfully!");
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to add edition.");
    }

    // Clear input fields after adding the edition
    ui->comboBoxBook->setCurrentIndex(-1);
    ui->lineEditPublisher->clear();
    ui->lineEditLanguage->clear();
    ui->lineEditSeries->clear();
    ui->spinBoxPageCount->clear();

    RefreshEditionCompleters(); // Refresh completers to include new entries
    ui->comboBoxBook->setFocus(); // Set focus back to book combo box

    RefreshEditionsView(); // Refresh the editions view to show the new edition

    RefreshMyLibraryCompleters();
}

void MainWindow::RefreshEditionCompleters()
{
    // Refresh combo box for books with their IDs, titles and authors
    if (!book_manager) {
        qCritical() << "BookManager is not initialized.";
        return;
    }
    if (!publisher_manager || !language_manager || !series_manager) {
        qCritical() << "IdNameTableManager instances are not initialized.";
        return;
    }
    
    QMap<int, QString> books = book_manager->GetAllBooks();
    ui->comboBoxBook->clear();
    for (auto it = books.constBegin(); it != books.constEnd(); ++it) {
        ui->comboBoxBook->addItem(it.value(), it.key());
    }

    // Refresh completers for edition-related input fields
    RefreshQCompleter(publisher_manager, ui->lineEditPublisher);
    RefreshQCompleter(language_manager, ui->lineEditLanguage);
    RefreshQCompleter(series_manager, ui->lineEditSeries);
}

void MainWindow::RefreshQCompleter(IdNameTableManager* manager, QLineEdit* lineEdit)
{
    if (!manager) {
        qCritical() << "IdNameTableManager is not initialized.";
        return;
    }

    QStringList names = manager->GetAllNames();
    QCompleter* completer = new QCompleter(names, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionRole(Qt::DisplayRole);
    lineEdit->setCompleter(completer);
}

void MainWindow::RefreshEditionsView()
{
    if (!edition_manager) {
        qCritical() << "EditionManager is not initialized.";
        return;
    }
    
    QMap<int, QString> editions = edition_manager->GetAllEditions();
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "Edition ID");
    model->setHeaderData(1, Qt::Horizontal, "Title");
    model->setHeaderData(2, Qt::Horizontal, "Publisher");
    model->setHeaderData(3, Qt::Horizontal, "Authors");

    int row = 0;
    for (auto it = editions.constBegin(); it != editions.constEnd(); ++it) {
        // Assuming label format: "Title - Publisher - Author1, Author2, ..."
        QStringList parts = it.value().split(" - ");
        QString title = parts.value(0);
        QString publisher = parts.value(1);
        QString authors = parts.value(2);

        QStandardItem* itemEditionId = new QStandardItem(QString::number(it.key()));
        QStandardItem* itemTitle = new QStandardItem(title);
        QStandardItem* itemPublisher = new QStandardItem(publisher);
        QStandardItem* itemAuthors = new QStandardItem(authors);

        model->setItem(row, 0, itemEditionId);
        model->setItem(row, 1, itemTitle);
        model->setItem(row, 2, itemPublisher);
        model->setItem(row, 3, itemAuthors);
        row++;
    }

    ui->tableViewEditions->setModel(model);
    ui->tableViewEditions->resizeColumnsToContents();

    // Hide the Edition ID column
    ui->tableViewEditions->setColumnHidden(0, true);
}

void MainWindow::RefreshMyLibraryCompleters()
{
    if (!my_library_manager || !shelf_manager) {
        qCritical() << "MyLibraryManager or ShelfManager is not initialized.";
        return;
    }

    QMap<int, QString> r_items = r_item_manager->GetAllRItems();
    ui->comboBoxRItem->clear();
    for (auto it = r_items.constBegin(); it != r_items.constEnd(); ++it) {
        ui->comboBoxRItem->addItem(it.value(), it.key());
    }

    // Refresh completers for MyLibrary-related input fields
    RefreshQCompleter(shelf_manager, ui->lineEditShelfName);
}

/// @todo Add QMessageBox to inform user about success or failure
/// @todo Edit input fields for more user-friendly experience
void MainWindow::on_pushButtonAddMyLibrary_clicked()
{
    if (!my_library_manager) {
        qCritical() << "MyLibraryManager is not initialized.";
        return;
    }

    MyLibraryData item_data;

    // Get the selected RItem
    item_data.r_item_id = ui->comboBoxRItem->currentData().toInt();
    if (!r_item_manager->RItemExists(item_data.r_item_id)) {
        qCritical() << "Invalid RItem selected.";
        return;
    }

    item_data.acquired_from = ui->lineEditAcquiredFrom->text().trimmed();

    item_data.acquired_date = ui->dateTimeEditAcquiredDate->dateTime();

    item_data.price = ui->doubleSpinBoxPrice->value();

    // Get the shelf name
    item_data.shelf_name = ui->lineEditShelfName->text();

    item_data.notes = ui->lineEditNotes->text();

    // Add the RItem to the MyLibrary
    my_library_manager->InsertRItem(item_data);

    RefreshMyLibraryCompleters(); // Refresh completers to include new entries
}

