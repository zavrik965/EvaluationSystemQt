#ifndef ADD_LESSON_DIALOG_H
#define ADD_LESSON_DIALOG_H

#include <QDialog>

namespace Ui {
class add_lesson_dialog;
}

class add_lesson_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit add_lesson_dialog(QWidget *parent = nullptr);
    ~add_lesson_dialog();

private:
    Ui::add_lesson_dialog *ui;

public:
    QStringList get_lesson();
};

#endif // ADD_LESSON_DIALOG_H
