#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    void setTitle(QString title);
    void setVersion(QString version);
    void setDescription(QString description);
    void setLink(QString link);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
