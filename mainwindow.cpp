#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QDebug>
#include "logindialog.h"
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include "coder.cpp"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    socket = new QTcpSocket();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(!tray_access){
        event->ignore();
        this->hide();
    }
}

void MainWindow::fullClose()
{
    ui->tray_access_btn->setChecked(false);
    tray_access = true;
    close();
}

void MainWindow::on_exit_triggered()
{
    fullClose();
}

void MainWindow::init()
{
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/pic/icon.png"), this);
    trayIcon->setToolTip("Система Зачётов");

    ui->description_field->setFontPointSize(14);
    ui->title_list->setFont(QFont("", 18));
    ui->excercises->setSpacing(2);

    connect(ui->menu_2, SIGNAL(triggered(QAction*)), SLOT(classes_event(QAction*)));

    on_change_theme_triggered();

    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction("Развернуть окно", this);
    QAction * quitAction = new QAction("Выход", this);

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(fullClose()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    //trayIcon->showMessage("Система зачётов", "Запуск..", QIcon(":/icons/pic/icon.png"), 1000);

    QString home_path = QDir::homePath();
    QDir directory(home_path + "/.СистемаЗачётов/");
    QFileInfoList list = directory.entryInfoList();
    QFile json_file;
    QString file_name;
    foreach(QFileInfo finfo, list)
    {
        if(finfo.isFile() && finfo.suffix() == "json"){
            file_name = finfo.fileName();
            qWarning() << home_path + "/.СистемаЗачётов/" + file_name;

            //******************
            //import json
            //******************

            QString val;
            json_file.setFileName(home_path + "/.СистемаЗачётов/" + file_name);
            json_file.open(QIODevice::ReadOnly | QIODevice::Text);
            val = json_file.readAll();
            json_file.close();
            QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
            lessons_data = d.array();
            lessons[Translit->fromTranslit(file_name.split(".")[0])] = lessons_data;
            ui->menu_2->addAction(Translit->fromTranslit(file_name.split(".")[0]));

            //******************
            //end import json
            //******************
         }
     }
}

void MainWindow::on_tray_access_btn_triggered()
{
    tray_access = !tray_access;
}

void MainWindow::on_change_theme_triggered()
{
    if(index_theme == 0){
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        ui->description_btn->setIcon(QIcon(":/icons/pic/description_2.png"));
        ui->code_viewer_btn->setIcon(QIcon(":/icons/pic/code_2.png"));
        ui->chat_viewer_btn->setIcon(QIcon(":/icons/pic/chat_2.png"));
        ui->download_btn->setIcon(QIcon(":/icons/pic/download_2.png"));
        ui->upload_btn->setIcon(QIcon(":/icons/pic/upload_2.png"));
        ui->description_field->setTextColor(QColor("white"));

        qApp->setPalette(darkPalette);
        index_theme=1;
    } else{
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(230, 230, 230));
        lightPalette.setColor(QPalette::WindowText, Qt::black);
        lightPalette.setColor(QPalette::Base, QColor(230, 230, 230));
        lightPalette.setColor(QPalette::AlternateBase, QColor(230, 230, 230));
        lightPalette.setColor(QPalette::ToolTipBase, Qt::black);
        lightPalette.setColor(QPalette::ToolTipText, Qt::black);
        lightPalette.setColor(QPalette::Text, Qt::black);
        lightPalette.setColor(QPalette::Button, QColor(230, 230, 230));
        lightPalette.setColor(QPalette::ButtonText, Qt::black);
        lightPalette.setColor(QPalette::BrightText, Qt::red);
        lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        lightPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        lightPalette.setColor(QPalette::HighlightedText, Qt::white);

        ui->description_btn->setIcon(QIcon(":/icons/pic/description.png"));
        ui->code_viewer_btn->setIcon(QIcon(":/icons/pic/code.png"));
        ui->chat_viewer_btn->setIcon(QIcon(":/icons/pic/chat.png"));
        ui->download_btn->setIcon(QIcon(":/icons/pic/download.png"));
        ui->upload_btn->setIcon(QIcon(":/icons/pic/upload.png"));
        ui->description_field->setTextColor(QColor("black"));

        qApp->setPalette(lightPalette);
        index_theme=0;
    }
    ui->description_field->setPlainText(ui->description_field->toPlainText());
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }
        break;
    default:
        break;
    }
}


void MainWindow::on_excercises_currentRowChanged(int currentRow)
{
    QJsonObject item = lessons_data.at(currentRow).toObject();
    current_description = item.value("description").toString();
    current_needed_files = item.value("file").toString().split("&");
    ui->description_field->setText(item.value("description").toString());
}


void MainWindow::on_description_btn_clicked()
{
    ui->description_field->setText(current_description);
}


void MainWindow::on_code_viewer_btn_clicked()
{
    ui->description_field->setText(current_answer_text);
}


void MainWindow::on_chat_viewer_btn_clicked()
{
    ui->description_field->setText(current_chat);
}

void MainWindow::classes_event(QAction * action)
{
    lessons_data = lessons[action->text()].toArray();
    current_description = "";
    current_answer_text = "";
    current_chat = "";
    current_needed_files = {};
    ui->excercises->clear();
    for(int i=0; i < lessons_data.size(); i++)
    {
        QJsonObject item = lessons_data.at(i).toObject();
        QListWidgetItem *newItem = new QListWidgetItem(ui->excercises);
        newItem->setTextAlignment(Qt::AlignCenter);
        newItem->setText(item.value("title").toString());
        newItem->setBackground(QColor(50, 50, 50));
        newItem->setForeground(QColor("white"));
        newItem->setFont(QFont("", 14));
        ui->excercises->addItem(newItem);
        ui->description_field->setText(current_description);
    }
}


void MainWindow::on_login_btn_triggered()
{
    LoginDialog * login_form = new LoginDialog();
    int answer = login_form->exec();
    if(answer == QDialog::Accepted){
        //******************
        //Connect to server
        QString home_path = QDir::homePath();
        QFile file;
        file.setFileName(home_path + "/.СистемаЗачётов/number");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        long long number = file.readAll().toLongLong();
        file.close();
        QString login_encode = encode(login_form->get_login(), number);
        QString password_encode = encode(login_form->get_password(), number);
        socket->connectToHost("alexavr.ru", 25555, QTcpSocket::ReadWrite);
        socket->write(QString("com://connect " + login_encode + "&&" + password_encode).toLocal8Bit());
        socket->waitForReadyRead(100);
        QByteArray data = socket->readAll();
        //
        //******************
        if(data == "ok"){
            number = gen(number);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            file.write(QString::number(number).toLocal8Bit());
            file.close();
            socket->waitForReadyRead(100);
            data = socket->readAll();
            person_type = data.split(' ')[0];
            class_num = Translit->fromTranslit(data.split(' ')[1]);
            login = login_form->get_login();
            username = "Заручевский\nАлександр";
            index_theme = 0;
            on_change_theme_triggered();
            ui->login_btn->setText("Сменить аккаунт");
            login_flag = true;
            delete login_form;
        }
        socket->close();
    }

    if(login_flag){
        login_flag = false;

        //******************
        //import files from ftp
        //******************
        ftp->Connect("alexavr.ru:2121");
        ftp->Login("u10", "10");

        ftp->Get("/tmp/avatar_img.jpg", ("avatars/" + Translit->toTranslit(username.replace("\n", "_") + "_" + class_num) + ".jpg").toLocal8Bit().constData(), ftplib::image);
        username.replace("_", "\n");
        ui->avatar->setPixmap(QPixmap("/tmp/avatar_img.jpg"));
        ui->username->setText(username);
        ui->class_num->setText(class_num);

        ftp->Nlst("/tmp/ftp_output", "student/" + class_num.left(class_num.size() - 1).toLocal8Bit() + "/");
        QString home_path = QDir::homePath();
        QString list;
        QFile file;
        file.setFileName("/tmp/ftp_output");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        list = file.readAll();
        file.close();
        QFile json_file;
        QStringList list_files = list.split('\n');
        QString file_name;
        ui->menu_2->clear();
        ui->excercises->clear();
        for(int i=0; i < list_files.size() - 1; i++)
        {
            if(list_files.at(i).split('/').at(2) != ""){
                file_name = list_files.at(i).split('/').at(2);
                qWarning() << home_path + "/.СистемаЗачётов/" + file_name;
                ftp->Get((home_path + "/.СистемаЗачётов/" + file_name).toLocal8Bit().constData(), ("student/" + class_num.left(class_num.size() - 1) + "/" + file_name).toLocal8Bit().constData(), ftplib::image);

                //******************
                //import json
                //******************

                QString val;
                json_file.setFileName(home_path + "/.СистемаЗачётов/" + file_name);
                json_file.open(QIODevice::ReadOnly | QIODevice::Text);
                val = json_file.readAll();
                json_file.close();
                QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
                lessons_data = d.array();
                lessons[Translit->fromTranslit(file_name.split("_")[0])] = lessons_data;
                ui->menu_2->addAction(Translit->fromTranslit(file_name.split("_")[0]));

                //******************
                //end import json
                //******************
             }
         }

         //******************
         //end import files from ftp
         //******************
    }
}

