#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <ftplib.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QActionGroup>
#include <QtNetwork/QTcpSocket>
#include "translitter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;

    bool tray_access=true;
    void init();
    QTcpSocket * socket;

    QSystemTrayIcon *trayIcon;
    Translitter * Translit = new Translitter();

    bool login_flag=false;
    QString login="";
    QString username="";
    QString class_num="";
    QString person_type="";

    ftplib *ftp = new ftplib();
    int index_theme=1;
    QString sh(string command)
    {
        string end = " > /tmp/sh_output ";
        char com[command.size() + end.size()];
        for(unsigned long i=0; i < command.size(); i++)
        {
            com[i] = command[i];
        }
        for(unsigned long i=0; i < end.size(); i++)
        {
            com[command.size() + i] = end[i];
        }
        system(com);

        QFile file("/tmp/sh_output");
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            return in.readAll();
        }
        file.close();
        return "";
    }

    QJsonArray lessons_data;
    QJsonObject lessons;
    QString current_description="";
    QString current_answer_text="";
    QString current_chat="";
    QStringList current_needed_files = {};

    void logining();

private slots:
    void on_exit_triggered();

    void on_change_theme_triggered();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_tray_access_btn_triggered();

    void on_excercises_currentRowChanged(int currentRow);

    void on_description_btn_clicked();

    void on_code_viewer_btn_clicked();

    void on_chat_viewer_btn_clicked();

    void classes_event(QAction * action);

    void fullClose();


    void on_login_btn_triggered();
};
#endif // MAINWINDOW_H
