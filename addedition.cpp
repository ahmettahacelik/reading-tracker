#include "addedition.h"
#include "ui_addedition.h"

#include <QFileDialog>

AddEdition::AddEdition(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddEdition)
{
    ui->setupUi(this);
}

AddEdition::~AddEdition()
{
    delete ui;
}

void AddEdition::on_pushButtonBrowseCoverImage_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select Cover Image"), QString(), tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (!file_name.isEmpty()) {
        ui->lineEditCoverImagePath->setText(file_name);

        QPixmap cover_image(file_name);
        ui->labelCoverImage->setPixmap(cover_image.scaled(ui->labelCoverImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

