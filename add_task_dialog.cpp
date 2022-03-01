#include "add_task_dialog.h"
#include "ui_add_task_dialog.h"

Add_task_dialog::Add_task_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Add_task_dialog)
{
    ui->setupUi(this);
    ui->text_task->hide();
    ui->title->hide();
    ui->title_label->hide();
    ui->files->hide();
    ui->file_label->hide();
}

Add_task_dialog::~Add_task_dialog()
{
    delete ui;
}

QStringList Add_task_dialog::get_task(){
    if(ui->task_type->currentIndex() == 1){
        return {"classic", ui->text_task->toPlainText(), ui->title->text(), ui->files->text()};
    } else if(ui->task_type->currentIndex() == 2){
        if(ui->task_list->count() != 0)
            return {"system", ui->task_list->currentItem()->text()};
        else{
            return {"", ""};
        }
    }
    return {"", ""};
}

void Add_task_dialog::on_task_type_currentIndexChanged(int index)
{
    if(index == 1){
        ui->task_list->hide();
        ui->text_task->show();
        ui->title->show();
        ui->title_label->show();
        ui->files->show();
        ui->file_label->show();
    } else if(index == 2){
       ui->task_list->show();
       ui->text_task->hide();
       ui->title->hide();
       ui->title_label->hide();
       ui->files->hide();
       ui->file_label->hide();
    }
}

