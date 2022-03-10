#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setTitle(QString title){
    ui->title->setText(title);
}

void AboutDialog::setVersion(QString version){
    ui->version->setText(version);
}

void AboutDialog::setDescription(QString description){
    ui->description->setText(description);
}

void AboutDialog::setLink(QString link){
    ui->link->setText("<a href=" + link + ">Сайт</a>");
}
