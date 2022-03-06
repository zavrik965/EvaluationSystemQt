#include "addtaskdialog.h"
#include "ui_addtaskdialog.h"

AddTaskDialog::AddTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTaskDialog)
{
    ui->setupUi(this);
    ui->task_list->hide();
    ui->text_task->hide();
    ui->title->hide();
    ui->title_label->hide();
    ui->files->hide();
    ui->file_label->hide();
}

AddTaskDialog::~AddTaskDialog()
{
    delete ui;
}

QStringList AddTaskDialog::get_task(){
    if(ui->task_type->currentIndex() == 1){
        return {"classic", ui->text_task->toPlainText(), ui->title->text(), ui->files->text()};
    } else if(ui->task_type->currentIndex() == 2){
        if(ui->task_list->count() != 0)
            return {"tasksystem", QString().number(ui->task_list->currentRow() + 1)};
        else{
            return {"", ""};
        }
    }
    return {"", ""};
}

void AddTaskDialog::add_task(QString task){
    ui->task_list->addItem(task);
}

void AddTaskDialog::on_task_type_currentIndexChanged(int index)
{
    if(index == 1){
        ui->task_list->hide();
        ui->text_task->show();
        ui->title->show();
        ui->title_label->show();
        ui->files->show();
        ui->file_label->show();
        ui->background->hide();
    } else if(index == 2){
       ui->task_list->show();
       ui->text_task->hide();
       ui->title->hide();
       ui->title_label->hide();
       ui->files->hide();
       ui->file_label->hide();
       ui->background->hide();
    } else{
       ui->task_list->hide();
       ui->text_task->hide();
       ui->title->hide();
       ui->title_label->hide();
       ui->files->hide();
       ui->file_label->hide();
       ui->background->show();
    }
}

