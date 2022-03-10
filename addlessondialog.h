#ifndef ADDLESSONDIALOG_H
#define ADDLESSONDIALOG_H

#include <QDialog>

namespace Ui {
class AddLessonDialog;
}

class AddLessonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLessonDialog(QWidget *parent = nullptr);
    ~AddLessonDialog();

private:
    Ui::AddLessonDialog *ui;

public:
    QStringList get_lesson();
};

#endif // ADDLESSONDIALOG_H
