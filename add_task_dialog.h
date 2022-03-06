#ifndef ADD_TASK_DIALOG_H
#define ADD_TASK_DIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class Add_task_dialog;
}

class Add_task_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Add_task_dialog(QWidget *parent = nullptr);
    ~Add_task_dialog();

private:
    Ui::Add_task_dialog *ui;
public:
    QStringList get_task();
    void add_task(QString task);
private slots:
    void on_task_type_currentIndexChanged(int index);
};

#endif // ADD_TASK_DIALOG_H
