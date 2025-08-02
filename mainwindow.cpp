#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCompleter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize EditionManager and BookManager
    database_manager = new DatabaseManager();
    id_name_table_manager = new IdNameTableManager(database_manager);
    book_manager = new BookManager(database_manager, id_name_table_manager);
    edition_manager = new EditionManager(database_manager, id_name_table_manager, book_manager);

    // Set up completers for input fields
    RefreshBookCompleters();

    RefreshEditionCompleters();
}

MainWindow::~MainWindow()
{
    delete edition_manager;
    delete book_manager;
    delete id_name_table_manager;
    delete database_manager;
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
    /// @todo Consider creating a function to RefreshCompleter which takes a table type and input field
    
    // Refresh completers for input fields
    QStringList authors = id_name_table_manager->GetAllNames(IdNameTable::Author);
    QCompleter* authorCompleter = new QCompleter(authors, this);
    authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    authorCompleter->setCompletionMode(QCompleter::PopupCompletion);
    authorCompleter->setFilterMode(Qt::MatchContains);
    authorCompleter->setCompletionRole(Qt::DisplayRole);
    ui->lineEditAuthors->setCompleter(authorCompleter);

    QStringList languages = id_name_table_manager->GetAllNames(IdNameTable::Language);
    QCompleter* languageCompleter = new QCompleter(languages, this);
    languageCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    languageCompleter->setCompletionMode(QCompleter::PopupCompletion);
    languageCompleter->setFilterMode(Qt::MatchContains);
    languageCompleter->setCompletionRole(Qt::DisplayRole);
    ui->lineEditOriginalLanguage->setCompleter(languageCompleter);

    QStringList countries = id_name_table_manager->GetAllNames(IdNameTable::Country);
    QCompleter* countryCompleter = new QCompleter(countries, this);
    countryCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    countryCompleter->setCompletionMode(QCompleter::PopupCompletion);
    countryCompleter->setFilterMode(Qt::MatchContains);
    countryCompleter->setCompletionRole(Qt::DisplayRole);
    ui->lineEditCountry->setCompleter(countryCompleter);

    QStringList genres = id_name_table_manager->GetAllNames(IdNameTable::Genre);
    QCompleter* genreCompleter = new QCompleter(genres, this);
    genreCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    genreCompleter->setCompletionMode(QCompleter::PopupCompletion);
    genreCompleter->setFilterMode(Qt::MatchContains);
    genreCompleter->setCompletionRole(Qt::DisplayRole);
    ui->lineEditGenres->setCompleter(genreCompleter);
}

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

    int edition_id = edition_manager->InsertEdition(edition_data);

    if (edition_id != -1) {
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
}

void MainWindow::RefreshEditionCompleters()
{
    // Refresh combo box for books with their IDs, titles and authors
    if (!book_manager) {
        qCritical() << "BookManager is not initialized.";
        return;
    }
    if (!id_name_table_manager) {
        qCritical() << "IdNameTableManager is not initialized.";
        return;
    }
    if (!database_manager || !database_manager->GetDatabase().isOpen()) {
        qCritical() << "Database connection is not valid or open.";
        return; // Database error
    }
    
    QMap<int, QString> books = book_manager->GetAllBooks();
    ui->comboBoxBook->clear();
    for (auto it = books.constBegin(); it != books.constEnd(); ++it) {
        ui->comboBoxBook->addItem(it.value(), it.key());
    }

    // Refresh completers for edition-related input fields
    QStringList publishers = id_name_table_manager->GetAllNames(IdNameTable::Publisher);
    QCompleter* publisherCompleter = new QCompleter(publishers, this);
    publisherCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    publisherCompleter->setCompletionMode(QCompleter::PopupCompletion);
    publisherCompleter->setFilterMode(Qt::MatchContains);
    ui->lineEditPublisher->setCompleter(publisherCompleter);

    QStringList languages = id_name_table_manager->GetAllNames(IdNameTable::Language);
    QCompleter* languageCompleter = new QCompleter(languages, this);
    languageCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    languageCompleter->setCompletionMode(QCompleter::PopupCompletion);
    languageCompleter->setFilterMode(Qt::MatchContains);
    ui->lineEditLanguage->setCompleter(languageCompleter);

    QStringList series = id_name_table_manager->GetAllNames(IdNameTable::Series);
    QCompleter* seriesCompleter = new QCompleter(series, this);
    seriesCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    seriesCompleter->setCompletionMode(QCompleter::PopupCompletion);
    seriesCompleter->setFilterMode(Qt::MatchContains);
    ui->lineEditSeries->setCompleter(seriesCompleter);
}

