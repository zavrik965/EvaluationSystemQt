#include "addlessondialog.h"
#include "ui_addlessondialog.h"

AddLessonDialog::AddLessonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLessonDialog)
{
    ui->setupUi(this);
}

AddLessonDialog::~AddLessonDialog()
{
    delete ui;
}

QStringList AddLessonDialog::get_lesson(){
    return {ui->new_lesson->text(), ui->new_class->text()};
}
