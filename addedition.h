#ifndef ADDEDITION_H
#define ADDEDITION_H

#include <QDialog>

namespace Ui {
class AddEdition;
}

class AddEdition : public QDialog
{
    Q_OBJECT

public:
    explicit AddEdition(QWidget *parent = nullptr);
    ~AddEdition();

private slots:
    void on_pushButtonBrowseCoverImage_clicked(); ///< Slot to handle the cover image browsing button click.

private:
    Ui::AddEdition *ui;
};

#endif // ADDEDITION_H
