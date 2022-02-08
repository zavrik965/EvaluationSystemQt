#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QDebug>
#include "logindialog.h"
#include <QColor>
#include <QFileInfo>
#include "coder.cpp"
#include <QTime>
#include <QScrollBar>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    timer = new QTimer(this);

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
    ui->chat_viewer->hide();
    ui->reconnect->setVisible(false);
    ui->chat_viewer->setAutoScroll(true);

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
    current_excersise = item.value("title").toString();
    current_description = item.value("description").toString();
    current_needed_files = item.value("file").toString().split("&");
    ui->description_field->setText(current_description);
    ui->chat_viewer->clear();
    ui->description_field->show();
    ui->chat_viewer->hide();
    QFile file;
    QString lesson = Translit->toTranslit(current_excersise);
    QDir dir(home_path + "/.СистемаЗачётов/.history/");
    QFileInfoList list = dir.entryInfoList();
    if(current_lesson.split('_').size() != 1){
        class_num = current_lesson.split('_')[1] + current_lesson.split('_')[2].toUpper();
    }
    foreach (QFileInfo finfo, list) {
        QString name = finfo.fileName();
        /*qWarning() << name;
        qWarning() << name.split('_');
        qWarning() << name.split('@') << Translit->toTranslit(class_num) << lesson.replace(" ", "_");*/
        if(name.split('_').size() > 1 && name.split('_')[3] == Translit->toTranslit(class_num) && name.split('@')[1].split('.')[0] == lesson.replace(" ", "_")){
            /*qWarning() << home_path + "/.СистемаЗачётов/.history/" + name;*/
            file.setFileName(home_path + "/.СистемаЗачётов/.history/" + name);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream in(&file);
            QString time, line, mes;
            QString text = file.readAll();
            for(int i=0; i < text.split('\n').size() - 1; i++){
                if(text.split('\n')[i].sliced(0, 5) != "$IAM$"){
                    /*qWarning() << text.split('\n')[i];*/
                    time = text.split('\n')[i].split('&')[0];
                    time = time.left(time.size() - 1);
                    mes = text.split('\n')[i].right(text.split('\n')[i].size() - time.size() - 3);
                    line = "[" + time + "] " + name.split('_')[2] + ":  " + mes.replace("#$#", "\n");
                    ui->chat_viewer->addItem(line);
                } else{
                    time = text.split('\n')[i].split('&')[0].split('$')[2];
                    time = time.left(time.size() - 1);
                    mes = text.split('\n')[i].right(text.split('\n')[i].size() - time.size() - 3 - 5);
                    line = mes.replace("#$#", "\n") + " [" + time + "]";
                    QListWidgetItem * mesItem = new QListWidgetItem(line);
                    mesItem->setTextAlignment(Qt::AlignRight);
                    ui->chat_viewer->addItem(mesItem);
                }
            }
            file.close();
        }
    }
    QScrollBar *vb = ui->chat_viewer->verticalScrollBar();
    vb->setValue(vb->maximum());
}


void MainWindow::on_description_btn_clicked()
{
    ui->description_field->setText(current_description);
    ui->description_field->show();
    ui->chat_viewer->hide();
}


void MainWindow::on_code_viewer_btn_clicked()
{
    ui->description_field->setText(current_answer_text);
    ui->description_field->show();
    ui->chat_viewer->hide();
}


void MainWindow::on_chat_viewer_btn_clicked()
{
    ui->description_field->hide();
    ui->chat_viewer->show();
}

void MainWindow::classes_event(QAction * action)
{
    lock = true;
    ui->excercises->blockSignals(true);
    ui->excercises->clear();
    ui->excercises->blockSignals(false);
    ui->chat_viewer->blockSignals(true);
    ui->chat_viewer->clear();
    ui->chat_viewer->blockSignals(false);
    lessons_data = lessons[action->text()].toArray();
    current_lesson = action->text();
    current_description = "";
    current_answer_text = "";
    current_needed_files = {};
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
    lock = false;
}


void MainWindow::on_login_btn_triggered()
{
    LoginDialog * login_form = new LoginDialog();
    int answer = login_form->exec();
    if(answer == QDialog::Accepted){
        //******************
        //Connect to server
        QFile file;
        file.setFileName(home_path + "/.СистемаЗачётов/number");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        long long number = file.readAll().toLongLong();
        file.close();
        QString login_encode = encode(login_form->get_login(), number);
        QString password_encode = encode(login_form->get_password(), number);
        qWarning() << login_encode;
        socket->connectToHost("alexavr.ru", 25555, QTcpSocket::ReadWrite);
        socket->waitForConnected();
        socket->write(QString("com://connect " + login_encode + "&&" + password_encode).toLocal8Bit());
        socket->waitForReadyRead();
        QByteArray data = socket->readAll();
        //
        //******************
        if(data == "ok"){
            number = gen(number);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            file.write(QString::number(number).toLocal8Bit());
            file.close();
            socket->waitForReadyRead();
            data = socket->readAll();
            person_type = data.split(' ')[0];
            class_num = Translit->fromTranslit(data.split(' ')[1]);
            login = login_form->get_login();
            username = "Заручевский\nАлександр";
            index_theme = 0;
            on_change_theme_triggered();
            ui->login_btn->setText("Сменить аккаунт");
            login_flag = true;
            ui->reconnect->setVisible(true);
            delete login_form;
            connectToMessageServer();
            reconnectingFTP();
            connect(timer, SIGNAL(timeout()), this, SLOT(reconnectingFTP()));
            timer->start(120000);
        }
        socket->close();
    }
}

void MainWindow::connectToMessageServer(){
    QByteArray data;
    if(login_flag){
        sock_send->connectToHost("alexavr.ru", 25545, QTcpSocket::ReadWrite);
        sock_send->waitForConnected();
        sock_send->write(QString("send&" + person_type + "_" + login + "_" + Translit->toTranslit(class_num)).toLocal8Bit());
        sock_send->waitForBytesWritten();
        sock_send->waitForReadyRead();
        data = sock_send->readAll();
        qWarning() << "OK?" <<  data;
        if(data != "Ok"){
            trayIcon->showMessage("Система зачётов", "Не удалось подключится к серверу сообщений", QIcon(":/icons/pic/icon.png"));
        } else{
            sock->connectToHost("alexavr.ru", 25545, QTcpSocket::ReadWrite);
            sock->waitForConnected();
            sock->write(QString("recv&" + person_type + "_" + login + "_" + Translit->toTranslit(class_num)).toLocal8Bit());
            sock->waitForBytesWritten();
            sock->waitForReadyRead();
            data = sock->readAll();
            qWarning() << "OK?" <<  data;
            if(data != "Ok"){
                trayIcon->showMessage("Система зачётов", "Не удалось подключится к серверу сообщений", QIcon(":/icons/pic/icon.png"));
            } else{
                connect(sock, SIGNAL(readyRead()), this, SLOT(reciver()));
                connect(ui->mes_btn, SIGNAL(clicked()), this, SLOT(sender()));
                qWarning() << "sock_conected";
            }
        }
    }
}
void MainWindow::reconnectingFTP(){
    if(login_flag){

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

        ftp->Nlst("/tmp/ftp_output", "student/" + class_num.left(class_num.size() - 1).toLocal8Bit() + "/tasks/");
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
        ui->chat_viewer->blockSignals(true);
        ui->chat_viewer->clear();
        ui->chat_viewer->blockSignals(false);
        ui->description_field->show();
        ui->chat_viewer->hide();
        for(int i=0; i < list_files.size() - 1; i++)
        {
            if(list_files.at(i).split('/').at(2) != ""){
                file_name = list_files.at(i).split('/').at(3);
                qWarning() << home_path + "/.СистемаЗачётов/" + file_name;
                ftp->Get((home_path + "/.СистемаЗачётов/" + file_name).toLocal8Bit().constData(), ("student/" + class_num.left(class_num.size() - 1) + "/tasks/" + file_name).toLocal8Bit().constData(), ftplib::image);

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
        if(current_lesson != ""){
            qWarning() << current_lesson;
            QAction * action = new QAction(current_lesson);
            classes_event(action);
        }
    }
}

void MainWindow::reciver(){
    QByteArray data;
    QString user;
    QFile file;
    QTime time = QTime::currentTime();
    data = sock->readAll();
    qWarning() << data;
    if(data != "ping@!#"){
        qWarning() << data;
        data = data.replace("#$#", "\n");
        user = data.split('%')[0];
        user = user.sliced(0, user.size() - 1);
        QString lesson = data.split('%')[1];
        lesson = lesson.sliced(1, lesson.size() - 2);
        if(data.split('%').size() >= 3){
            qWarning() << home_path + "/.СистемаЗачётов/.history/chat_" + user + "_" + lesson + ".hs";
            file.setFileName(home_path + "/.СистемаЗачётов/.history/chat_" + user + "_" + lesson + ".hs");
            file.open(QIODevice::Append | QIODevice::Text);

            file.write(QString(time.toString("hh:mm:ss") + ":&" + data.sliced(user.size() + lesson.size() + 7).replace('\n', "#$#") + "\n").toLocal8Bit());
            if(!lock && ui->excercises->count() != 0 && ui->excercises->currentItem()){
                if(!lock && lesson.split('@')[0] == Translit->toTranslit(current_lesson) && lesson.split('@')[1].replace("_", " ") == Translit->toTranslit(ui->excercises->currentItem()->text())){
                    //qWarning() << Translit->toTranslit(ui->excercises->currentItem()->text()) << Translit->toTranslit(current_lesson) << lesson.split('@')[1].replace("_", " ") << lesson.split('@')[0];
                    ui->chat_viewer->addItem("[" + time.toString("hh:mm:ss") + "] " + user + ": " + data.sliced(user.size() + lesson.size() + 7));
                }else {
                    trayIcon->showMessage("Система зачётов", "Новое сообщение от пользователя " + Translit->fromTranslit(user.split("_")[1]), QIcon(":/icons/pic/icon.png"));
                }
            } else {
                trayIcon->showMessage("Система зачётов", "Новое сообщение от пользователя " + Translit->fromTranslit(user.split("_")[1]), QIcon(":/icons/pic/icon.png"));
            }
        }
    }
    file.close();
}

void MainWindow::sender(){
    if(sock_send->isOpen() && !ui->chat_viewer->isHidden()){
        //************
        // add to file
        //************
        QFile file;
        QDir dir(home_path + "/.СистемаЗачётов/.history/");
        QFileInfoList list = dir.entryInfoList();
        QTime time = QTime::currentTime();
        foreach (QFileInfo finfo, list) {
            QString name = finfo.fileName();
            if(name.split('_').size() >= 5) qWarning() << name << name.split('_')[4].split('@')[0] << Translit->toTranslit(current_lesson) << name.split('@')[1].split('.')[0].replace("_", " ") << Translit->toTranslit(current_excersise);
            if(name.split('_').size() >= 5 && name.split('_')[3] == Translit->toTranslit(class_num) && name.split('_')[4].split('@')[0] == Translit->toTranslit(current_lesson) && name.split('@')[1].split('.')[0].replace("_", " ") == Translit->toTranslit(current_excersise)){
                qWarning() << "[TO:" + name.split('_')[1] + "_" + name.split('_')[2] + "_" + name.split('_')[3] + "]&:&" + current_lesson + "@" + current_excersise + ":%:" + ui->message->toPlainText().replace('\n', "#$#");
                sock_send->write(QString("[TO:" + name.split('_')[1] + "_" + name.split('_')[2] + "_" + name.split('_')[3] + "]&:&" + Translit->toTranslit(current_lesson) + "@" + Translit->toTranslit(current_excersise).replace(" ", "_") + ":%:" + ui->message->toPlainText().replace('\n', "#$#")).toLocal8Bit());
                sock_send->waitForBytesWritten();
                file.setFileName(home_path + "/.СистемаЗачётов/.history/" + name);
                file.open(QIODevice::Append | QIODevice::Text);
                file.write(QString("$IAM$" + time.toString("hh:mm:ss") + ":&:" + ui->message->toPlainText().replace('\n', "#$#") + "\n").toLocal8Bit());
                file.waitForBytesWritten(1000);
                file.close();
            }
        }
        //************
        // end
        //************
        QListWidgetItem * mes = new QListWidgetItem(ui->message->toPlainText() + " [" + time.toString("hh:mm:ss") + "]");
        mes->setTextAlignment(Qt::AlignRight);
        ui->chat_viewer->addItem(mes);
        ui->message->setText("");
    } else {
        trayIcon->showMessage("Система зачётов", "Нет соединения с сервером сообщений", QIcon(":/icons/pic/icon.png"));
    }
}


void MainWindow::on_reconnect_triggered()
{
    //sock_send->write("disconnect");
    //sock_send->waitForBytesWritten();
    disconnect(sock, SIGNAL(readyRead()), this, SLOT(reciver()));
    sock_send->close();
    sock->close();
    reconnectingFTP();
    connectToMessageServer();
}


void MainWindow::on_upload_btn_clicked()
{
    if(username != "" && current_needed_files.size() != 0){
        if(ui->chat_viewer->isHidden()){
            QString file_types="";
            for(int i=0; i < current_needed_files.size(); i++){
                file_types += "*." + current_needed_files[i].split("_")[1] + " ";
            }
            qWarning() << file_types << current_needed_files;
            QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Send files"), home_path, tr(QString("Files " + file_types + "").toLocal8Bit()));
            qWarning() << fileNames;
            QString end;
            QMap<QString, int> count_fileends;
            foreach(QString filename, fileNames){
                end = filename.split(".")[filename.split(".").size() - 1];
                if(count_fileends.find(end) != count_fileends.end()){
                    count_fileends[end] += 1;
                } else{
                    count_fileends[end] = 1;
                }
            }
            qWarning() << count_fileends << current_needed_files;
            QString error_mes = "";
            foreach(QString file, current_needed_files){
                if(count_fileends[file.split("_")[1]] != file.split("_")[0].toInt()){
                    error_mes += file.split("_")[1] + ": " + QString::number(count_fileends[file.split("_")[1]]) + "/" + file.split("_")[0] + "\n";
                }
            }
            if(error_mes != ""){
                QMessageBox::critical(this, "Ошибка отправки задания", "Вы выбрали неверное количество файлов:\n" + error_mes);
            } else{
                QString file_path = tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username.replace("\n", "_")) + ".zip";
                QZipWriter writer(file_path);
                for(int i=0; i < fileNames.size(); i++){
                    QString filename=fileNames.at(i);
                    QFile file(filename);
                    if(file.exists()){
                        file.open(QIODevice::ReadOnly);
                        writer.addFile(filename.split('/').takeLast(), file.readAll());
                        file.close();
                    }
                }
                writer.close();
                int code = ftp->Put(file_path.toLocal8Bit(), QString(person_type + "/" + class_num.left(class_num.size() - 1) + "/ready_tasks/" + file_path.split('/').takeLast()).toLocal8Bit(), ftplib::image);
                if(code == 1){
                    trayIcon->showMessage("Система зачётов", "Задание отправлено на проверку", QIcon(":/icons/pic/icon.png"));
                } else{
                    trayIcon->showMessage("Система зачётов", "Задание не было отправлено на проверку, проверьте соединение с сервером", QIcon(":/icons/pic/icon.png"));
                }
            }
        }
    }
}

