#include "add_lesson_dialog.h"
#include "ui_add_lesson_dialog.h"

add_lesson_dialog::add_lesson_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_lesson_dialog)
{
    ui->setupUi(this);
}

add_lesson_dialog::~add_lesson_dialog()
{
    delete ui;
}

QStringList add_lesson_dialog::get_lesson(){
    return {ui->new_lesson->text(), ui->new_class->text()};
}
