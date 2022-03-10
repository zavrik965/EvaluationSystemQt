#ifndef ADDTASKDIALOG_H
#define ADDTASKDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class AddTaskDialog;
}

class AddTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTaskDialog(QWidget *parent = nullptr);
    ~AddTaskDialog();

private:
    Ui::AddTaskDialog *ui;
public:
    QStringList get_task();
    void add_task(QString task);
    void remove_task(int task);
private slots:
    void on_task_type_currentIndexChanged(int index);
};

#endif // ADDTASKDIALOG_H
