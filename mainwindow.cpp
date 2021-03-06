#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QDebug>
#include <QColor>
#include <QFileInfo>
#include <QTime>
#include <QScrollBar>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopServices>
#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"
#include "logindialog.h"
#include "addtaskdialog.h"
#include "addlessondialog.h"
#include "coder.cpp"


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
    delete about_dialog;
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
    QDir main_dir(QDir::homePath());
    main_dir.mkdir(".СистемаЗачётов");
    main_dir.setPath(QDir::homePath() + "/.СистемаЗачётов");
    main_dir.mkdir("tmp");
    main_dir.mkdir(".history");

    trayIcon = new QSystemTrayIcon(QIcon(":/icons/pic/icon.png"), this);
    trayIcon->setToolTip("Система Зачётов");

    QFile versionfile(":/settings/version");
    versionfile.open(QFile::ReadOnly);
    QString data = versionfile.readAll();
    about_dialog->setTitle(data.split("\n")[0]);
    about_dialog->setVersion(data.split("\n")[1]);
    about_dialog->setDescription(data.split("\n")[2]);
    about_dialog->setLink(data.split("\n")[3]);
    versionfile.close();

    ui->ready_tasks_list->setSpacing(2);
    ui->excercises->setSpacing(2);
    ui->chat_viewer->hide();
    ui->answer_field->hide();
    ui->ready_tasks_list->hide();
    ui->list_students_btn->hide();
    ui->open_folder_btn->hide();
    ui->teacher_mark_widget->hide();
    ui->add_task_btn->hide();
    ui->add_lesson->hide();
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
    if(person_type == "student"){
        QDir directory(home_path + "/.СистемаЗачётов/");
        QFileInfoList list = directory.entryInfoList();
        QFile json_file;
        QString file_name;
        foreach(QFileInfo finfo, list)
        {
            if(finfo.isFile() && finfo.suffix() == "json"){
                file_name = finfo.fileName();

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
        ui->upload_btn->setIcon(QIcon(":/icons/pic/upload_2.png"));
        ui->list_students_btn->setIcon(QIcon(":/icons/pic/list_students_2.png"));
        ui->open_folder_btn->setIcon(QIcon(":/icons/pic/folder_2.png"));
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
        ui->upload_btn->setIcon(QIcon(":/icons/pic/upload.png"));
        ui->list_students_btn->setIcon(QIcon(":/icons/pic/list_students.png"));
        ui->open_folder_btn->setIcon(QIcon(":/icons/pic/folder.png"));
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


void MainWindow::on_ready_tasks_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current){
        ui->teacher_mark_widget->show();
        ui->answer_field->setText("");
        ui->open_folder_btn->show();
        //qWarning() << current_files[current->text()] << tmp_path + current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast();
        QZipReader zip(current_files[current->text()]);
        QDir dir(tmp_path);
        dir.mkdir(current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast());
        zip.extractAll(tmp_path + "/" + current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast());
        ui->answer_field->show();
        QString text;
        foreach(QZipReader::FileInfo data, zip.fileInfoList()){
            QFile file;
            QString d;
            QString href = tmp_path + "/" + current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast() + "/" + data.filePath;
            if(text_types.filter(QFileInfo(tmp_path + "/" + current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast() + "/" + data.filePath).suffix()).size() != 0){
                file.setFileName(tmp_path + "/" + current_files[current->text()].left(current_files[current->text()].size() - 4).split("/").takeLast() + "/" + data.filePath);
                file.open(QIODevice::ReadWrite | QIODevice::Text);
                d = file.readAll();
                d = d.replace("\n", "<br>");
                file.close();
            } else {
                d = "Данный файл недоступен для предпросмотра";
            }
            for(int i=0; i < data.filePath.size(); i++){
                text += "#";
            }
            QString link = "##<a href=\"" + href + "\">" + data.filePath + "</a>##";
            text += "<br>" + link + "<br>";
            for(int i=0; i < data.filePath.size(); i++){
                text += "#";
            }
            text += "<br><br>";
            text += d;
            text += "<br><br>";
        }
        ui->answer_field->append(text);
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
    ui->answer_field->hide();
    ui->description_field->show();
    ui->teacher_mark_widget->hide();
    ui->chat_viewer->hide();
    ui->open_folder_btn->hide();
    ui->ready_tasks_list->hide();
    if(person_type == "teacher"){
        ui->upload_btn->hide();
    }
    QFile file;
    QString num_class;
    QString lesson = Translit->toTranslit(current_excersise);
    QDir dir(home_path + "/.СистемаЗачётов/.history/");
    QFileInfoList list = dir.entryInfoList();
    if(current_lesson.split('_').size() != 1){
        num_class = current_lesson.split('_')[1] + current_lesson.split('_')[2].toUpper();
    } else if(person_type == "student"){
        num_class = class_num;
    }
    foreach (QFileInfo finfo, list) {
        QString name = finfo.fileName();
        if(finfo.isFile() && finfo.suffix() == "hs"){
            /*qWarning() << name;
            qWarning() << name.split('_');
            qWarning() << name.split('@') << Translit->toTranslit(class_num) << lesson.replace(" ", "_");*/
            if(name.split('_').size() > 1 && name.split('_')[3] == Translit->toTranslit(num_class) && name.split('@')[1].split('.')[0] == lesson.replace(" ", "_")){
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
        } else if(finfo.isFile() && finfo.suffix() == "json"){
            if(name.split('_').size() > 1 && name.split('_')[2].split('@')[0].replace("_", "") == Translit->toTranslit(num_class) && name.split('@')[1].split('.')[0] == lesson.replace(" ", "_")){
                QFile json;
                QString val;
                json.setFileName(home_path + "/.СистемаЗачётов/.history/" + name);
                json.open(QIODevice::ReadOnly |QIODevice::Text);
                val = json.readAll();
                json.close();
                QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
                //qWarning() << d;
                foreach(QJsonValueRef raw_message, d.array()){
                    QJsonObject message = raw_message.toObject();
                    if(message["from"] == person_type + "_" + login + "_" + Translit->toTranslit(class_num).replace("_", "")){
                        QListWidgetItem * mesItem = new QListWidgetItem(message["text"].toString());
                        mesItem->setTextAlignment(Qt::AlignRight);
                        ui->chat_viewer->addItem(mesItem);
                    } else{
                        QListWidgetItem * mesItem = new QListWidgetItem(message["text"].toString());
                        mesItem->setTextAlignment(Qt::AlignLeft);
                        ui->chat_viewer->addItem(mesItem);
                    }
                }
            }
        }
    }
    QScrollBar *vb = ui->chat_viewer->verticalScrollBar();
    vb->setValue(vb->maximum());
}


void MainWindow::on_description_btn_clicked()
{
    ui->description_field->clear();
    ui->description_field->setText(current_description);
    ui->description_field->show();
    ui->ready_tasks_list->hide();
    ui->chat_viewer->hide();
    ui->open_folder_btn->hide();
    ui->answer_field->hide();
    ui->teacher_mark_widget->hide();
    if(person_type == "teacher"){
        ui->upload_btn->hide();
    }
}


void MainWindow::on_code_viewer_btn_clicked()
{
    ui->description_field->hide();
    ui->answer_field->show();
    ui->chat_viewer->hide();
    ui->ready_tasks_list->hide();
    ui->teacher_mark_widget->hide();
    if(person_type == "student" && login_flag){
        ui->answer_field->clear();
        ui->open_folder_btn->hide();
        QString filename = tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username + ".zip");
        if(QFile().exists(filename)){
            ui->answer_field->setText("");
            ui->open_folder_btn->show();
            ui->answer_field->show();

            //filename = home_path + "/" + filename;
            qWarning() << filename;
            QZipReader zip(filename);
            QDir dir(tmp_path);
            QDir temp_dir(tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username));
            temp_dir.removeRecursively();
            dir.mkdir(tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username));
            zip.extractAll(tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username));

            QString text;
            foreach(QZipReader::FileInfo data, zip.fileInfoList()){
                QFile file;
                QString d;
                QString href = tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username) + "/" + data.filePath;
                if(text_types.filter(QFileInfo(tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username) + "/" + data.filePath).suffix()).size() != 0){
                    file.setFileName(tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username) + "/" + data.filePath);
                    file.open(QIODevice::ReadWrite | QIODevice::Text);
                    d = file.readAll();
                    d = d.replace("\n", "<br>");
                    file.close();
                } else {
                    d = "Данный файл недоступен для предпросмотра";
                }
                for(int i=0; i < data.filePath.size(); i++){
                    text += "#";
                }
                QString link = "##<a href=\"" + href + "\">" + data.filePath + "</a>##";
                text += "<br>" + link + "<br>";
                for(int i=0; i < data.filePath.size(); i++){
                    text += "#";
                }
                text += "<br><br>";
                text += d;
                text += "<br><br>";
            }
            ui->answer_field->append(text);
        }
        //ui->answer_field->setText(current_answer_text);
    } else if(person_type == "teacher"){
        ui->upload_btn->hide();
    }
}


void MainWindow::on_chat_viewer_btn_clicked()
{
    ui->description_field->hide();
    ui->ready_tasks_list->hide();
    ui->chat_viewer->show();
    ui->open_folder_btn->hide();
    ui->answer_field->hide();
    ui->upload_btn->show();
    ui->teacher_mark_widget->hide();
}


void MainWindow::on_list_students_btn_clicked()
{
    ui->description_field->hide();
    ui->chat_viewer->hide();
    ui->answer_field->hide();
    ui->ready_tasks_list->show();
    ui->open_folder_btn->hide();
    ui->upload_btn->hide();

    QString num_class = "";
    QStringList list;
    if(current_lesson != "" && ui->excercises->currentItem() != 0x0){
        num_class = current_lesson.split("_")[1];
        string tmp_line;
        ftp->List(QString("student/" + num_class + "/ready_tasks/").toStdString(), tmp_line);
        foreach(QString filename, QString().fromStdString(tmp_line).split("\n")){
            filename = "student/" + num_class + "/ready_tasks/" + filename;
            if(filename.split("$").size() == 3 &&
                    filename.split("$")[2][num_class.size()] == Translit->toTranslit(current_lesson.split("_")[2]) &&
                    filename.split("$")[0].split("/").takeLast() == Translit->toTranslit(current_lesson.split("_")[0]) &&
                    filename.split("$")[1] == Translit->toTranslit(ui->excercises->currentItem()->text().replace(" ", "_"))){
                list += filename;
            }
        }

    }
    else if(current_lesson != ""){
        num_class = current_lesson.split("_")[1];
        string tmp_line;
        ftp->List(QString("student/" + num_class + "/ready_tasks/").toStdString(), tmp_line);
        foreach(QString filename, QString().fromStdString(tmp_line).split("\n")){
            filename = "student/" + num_class + "/ready_tasks/" + filename;
            if(filename.split("$").size() == 3 && filename.split("$")[2][num_class.size()] == Translit->toTranslit(current_lesson.split("_")[2]) && filename.split("$")[0].split("/").takeLast() == Translit->toTranslit(current_lesson.split("_")[0])){
                list += filename;
            }
        }
    } else{
        foreach(QString num_class, class_num.split(";")){
            string tmp_line;
            ftp->List(QString("student/" + num_class.left(num_class.size() - 1) + "/ready_tasks/").toStdString(), tmp_line);
            foreach(QString filename, QString().fromStdString(tmp_line).split("\n")){
                filename = "student/" + num_class.left(num_class.size() - 1) + "/ready_tasks/" + filename;
                if(filename.split("$").size() == 3 && filename.split("$")[2][num_class.left(num_class.size() - 1).size()] == Translit->toTranslit(num_class.right(1))){
                    list += filename;
                }
            }
        }
    }
    //qWarning() << list;
    ui->ready_tasks_list->clear();
    foreach(QString filename, list){
        ftp->DownloadFile((tmp_path + "/" + filename.split("/").takeLast()).toStdString(), filename.toStdString());
        QString task = filename.split("/").takeLast().split(".")[0];
        QListWidgetItem *newItem = new QListWidgetItem(ui->ready_tasks_list);
        newItem->setTextAlignment(Qt::AlignCenter);
        //qWarning() << task.split("$")[2];
        newItem->setText(Translit->fromTranslit(task.split("$")[1].replace("_", " ") + ": " + task.split("$")[2].split("_")[1] + " " + task.split("$")[2].split("_")[2]));
        newItem->setBackground(QColor(50, 50, 50));
        newItem->setForeground(QColor("white"));
        newItem->setFont(QFont("", 12));
        ui->ready_tasks_list->addItem(newItem);
        current_files[Translit->fromTranslit(task.split("$")[1].replace("_", " ") + ": " + task.split("$")[2].split("_")[1] + " " + task.split("$")[2].split("_")[2])] = tmp_path + "/" + filename.split("/").takeLast();
    }

}


void MainWindow::on_open_folder_btn_clicked()
{
    if(person_type == "teacher"){
        //Windows
        QProcess::startDetached("explorer", QStringList() << tmp_path + "/" + current_files[ui->ready_tasks_list->currentItem()->text()].left(current_files[ui->ready_tasks_list->currentItem()->text()].size() - 4).split("/").takeLast());
        //Linux
        QProcess::startDetached("xdg-open", QStringList() << tmp_path + "/" + current_files[ui->ready_tasks_list->currentItem()->text()].left(current_files[ui->ready_tasks_list->currentItem()->text()].size() - 4).split("/").takeLast());
    } else if(person_type == "student"){
        //Windows
        QProcess::startDetached("explorer", QStringList() << tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username));
        //Linux
        QProcess::startDetached("xdg-open", QStringList() << tmp_path + "/" + Translit->toTranslit(current_lesson.replace(" ", "_") + "$" + current_excersise.replace(" ", "_") + "$" + class_num + "_" + username));
    }
}

void MainWindow::on_answer_field_anchorClicked(const QUrl &link)
{
    QProcess::startDetached("xdg-open", QStringList() << link.toString());
}


void MainWindow::classes_event(QAction * action)
{
    ui->open_folder_btn->hide();
    ui->teacher_mark_widget->hide();
    lock = true;
    ui->excercises->blockSignals(true);
    ui->excercises->clear();
    ui->excercises->blockSignals(false);
    ui->chat_viewer->blockSignals(true);
    ui->chat_viewer->clear();
    ui->chat_viewer->blockSignals(false);
    ui->ready_tasks_list->blockSignals(true);
    ui->ready_tasks_list->clear();
    ui->ready_tasks_list->blockSignals(false);
    ui->answer_field->clear();
    if(person_type == "teacher"){
        ui->upload_btn->hide();
    }
    lessons_data = lessons[action->text()].toArray();
    current_lesson = action->text();
    current_description = "";
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
        file.setFileName(home_path + "/.СистемаЗачётов/number_" + login_form->get_login());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        long long number = file.readAll().toLongLong();
        file.close();
        QString login_encode = encode(login_form->get_login(), number);
        QString password_encode = encode(login_form->get_password(), number);
        //qWarning() << login_encode;
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
            username = Translit->fromTranslit(data.split(' ')[2]);
            username.replace("_", "\n");
            if(data.split(' ').size() == 4){
                file.open(QIODevice::WriteOnly | QIODevice::Text);
                file.write(data.split(' ')[3]);
                file.close();
            }
            ui->username->setText(username);
            ui->class_num->setText(class_num);
            index_theme = 0; //ВРЕМЕННО
            on_change_theme_triggered();
            ui->login_btn->setText("Сменить аккаунт");
            login_flag = true;
            ui->reconnect->setVisible(true);
            delete login_form;
            if(sock_send->ConnectedState){
                sock_send->close();
            }
            if(sock->ConnectedState){
                sock->close();
            }
            connectToMessageServer();
            reconnectingFTP();
            connect(timer, SIGNAL(timeout()), this, SLOT(reconnectingFTP()));
            timer->start(120000);
        } else{
            QMessageBox::critical(this, "Ошибка аутентификации", "Вы ввели неверный пароль или логин");
        }
        socket->close();
        if(person_type == "teacher"){
            ui->list_students_btn->show();
            ui->teacher_mark_widget->hide();
            ui->add_task_btn->show();
            ui->add_lesson->show();

            ui->upload_btn->hide();
        } else if(person_type == "student"){
            ui->list_students_btn->hide();
            ui->teacher_mark_widget->hide();
            ui->add_task_btn->hide();
            ui->add_lesson->hide();

            ui->upload_btn->show();
        }
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
    if(person_type == "student"){
        if(login_flag){

            //******************
            //import files from ftp
            //******************
            ftp->InitSession("alexavr.ru", 2121, "u10", "10");
            //ftp->Connect("alexavr.ru:2121");
            //ftp->Login("u10", "10");
            ftp->DownloadFile(QString(QDir::tempPath() + "/avatar_img.jpg").toStdString(), ("avatars/" + Translit->toTranslit(username.replace("\n", "_") + "_" + class_num) + ".jpg").toStdString());
            ui->avatar->setPixmap(QPixmap(QString(QDir::tempPath() + "/avatar_img.jpg").toLocal8Bit()));
            string list;
            ftp->List(QString("student/" + class_num.left(class_num.size() - 1).toLocal8Bit() + "/tasks/").toStdString(), list);
            /*QString list;
            QFile file;
            file.setFileName(QString(QDir::tempPath() + "/ftp_output").toLocal8Bit());
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            list = file.readAll();
            file.close();*/
            QFile json_file;
            QStringList list_files = QString().fromStdString(list).split('\n');
            QString file_name;
            ui->menu_2->clear();
            ui->chat_viewer->blockSignals(true);
            ui->chat_viewer->clear();
            ui->chat_viewer->blockSignals(false);
            ui->description_field->show();
            ui->chat_viewer->hide();
            ui->answer_field->hide();
            ui->ready_tasks_list->hide();
            for(int i=0; i < list_files.size() - 1; i++)
            {
                QString file = "student/" + class_num.left(class_num.size() - 1).toLocal8Bit() + "/tasks/" + list_files.at(i);
                if(file.split('/').at(2) != "" && file.split('/').at(3).split('.')[0].endsWith(class_num.left(class_num.size() - 1) + "_" + Translit->toTranslit(class_num[class_num.size() - 1]))){
                    file_name = file.split('/').at(3);
                    ftp->DownloadFile((home_path + "/.СистемаЗачётов/" + file_name).toStdString(), ("student/" + class_num.left(class_num.size() - 1) + "/tasks/" + file_name).toStdString());

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

            QStringList filelist;
            string tmp_line;
            ftp->List(QString("student/" + class_num.left(class_num.size() - 1) + "/ready_tasks/").toStdString(), tmp_line);
            foreach(QString filename, QString().fromStdString(tmp_line).split("\n")){
                filename = "student/" + class_num.left(class_num.size() - 1) + "/ready_tasks/" + filename;
                if(filename.split("$").size() == 3 && filename.split("$").takeLast() == Translit->toTranslit(class_num + "_" + username) + ".zip"){
                    filelist += filename;
                }
            }
            foreach(QString filename, filelist){
                ftp->DownloadFile((tmp_path + "/" + filename.split("/").takeLast()).toStdString(), filename.toStdString());
                QString task = filename.split("/").takeLast().split(".")[0];
                current_files[Translit->fromTranslit(task.split("$")[1].replace("_", " ") + ": " + task.split("$")[2].split("_")[1] + " " + task.split("$")[2].split("_")[2])] = tmp_path + "/" + filename.split("/").takeLast();
            }
             //******************
             //end import files from ftp
             //******************
            if(current_lesson != ""){
                QAction * action = new QAction(current_lesson);
                classes_event(action);
            }
        }
    } else if(person_type == "teacher"){
        if(login_flag){

            //******************
            //import files from ftp
            //******************
            ftp->InitSession("alexavr.ru", 2121, "u10", "10");
            ftp->DownloadFile(QString(QDir::tempPath() + "/avatar_img.jpg").toStdString(), ("avatars/teacher_" + Translit->toTranslit(username.replace("\n", "_")) + ".jpg").toStdString());
            ui->avatar->setPixmap(QPixmap(QDir::tempPath() + "/avatar_img.jpg"));
            lessons = QJsonObject();
            ui->menu_2->clear();
            ui->chat_viewer->blockSignals(true);
            ui->chat_viewer->clear();
            ui->chat_viewer->blockSignals(false);
            ui->description_field->show();
            ui->chat_viewer->hide();
            ui->answer_field->hide();
            ui->ready_tasks_list->hide();
            foreach(QString num_class, class_num.split(";")){
                string list;
                ftp->List(QString("student/" + num_class.left(num_class.size() - 1).toLocal8Bit() + "/tasks/").toStdString(), list);
                QFile json_file;
                QStringList list_files = QString().fromStdString(list).split('\n');
                QString file_name;
                for(int i=0; i < list_files.size() - 1; i++)
                {
                    QString file = "student/" + num_class.left(num_class.size() - 1).toLocal8Bit() + "/tasks/" + list_files.at(i);
                    if(file.split('/').at(2) != "" && file.split('/').at(3).split('.')[0].endsWith(num_class.left(num_class.size() - 1) + "_" + Translit->toTranslit(num_class[num_class.size() - 1]))){
                        file_name = file.split('/').at(3);
                        ftp->DownloadFile((home_path + "/.СистемаЗачётов/" + file_name).toStdString(), ("student/" + num_class.left(num_class.size() - 1) + "/tasks/" + file_name).toStdString());

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
                        lessons[Translit->fromTranslit(file_name.split('.')[0])] = lessons_data;
                        ui->menu_2->addAction(Translit->fromTranslit(file_name.split('.')[0]));
                        //******************
                        //end import json
                        //******************
                     }
                 }

                 //******************
                 //end import files from ftp
                 //******************
            }
            if(current_lesson != ""){
                QAction * action = new QAction(current_lesson);
                classes_event(action);
            }
        }
    }
}

void MainWindow::reciver(){
    QByteArray data;
    QString user;
    int id;
    QFile file;
    QDateTime time = QDateTime::currentDateTime();
    QString strtime = time.toString("dd.MM.yyyy hh:mm:ss");
    data = sock->readAll();
    //qWarning() << data;
    if(data != "ping@!#"){
        qWarning() << data;
        data = data.replace("#$#", "\n");
        id = data.split(';')[0].toInt();
        user = data.split('%')[0].split(';')[1];
        user = user.sliced(0, user.size() - 1);
        QString lesson = data.split('%')[1];
        lesson = lesson.sliced(1, lesson.size() - 2);
        if(data.split('%').size() >= 3){
            //qWarning() << home_path + "/.СистемаЗачётов/.history/chat_" + user + "_" + lesson + ".hs";
            file.setFileName(home_path + "/.СистемаЗачётов/.history/chat_" + lesson + ".json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);

//            file.write(QString(time.toString("hh:mm:ss") + ":&" + data.sliced(user.size() + lesson.size() + 7).replace('\n', "#$#") + "\n").toLocal8Bit());
//            if(!lock && ui->excercises->count() != 0 && ui->excercises->currentItem()){
//                if(!lock && lesson.split('@')[0] == Translit->toTranslit(current_lesson) && lesson.split('@')[1].replace("_", " ") == Translit->toTranslit(ui->excercises->currentItem()->text())){
//                    //qWarning() << Translit->toTranslit(ui->excercises->currentItem()->text()) << Translit->toTranslit(current_lesson) << lesson.split('@')[1].replace("_", " ") << lesson.split('@')[0];
//                    ui->chat_viewer->addItem("[" + time.toString("hh:mm:ss") + "] " + user + ": " + data.sliced(user.size() + lesson.size() + 7));
//                }else {
//                    trayIcon->showMessage("Система зачётов", "Новое сообщение от пользователя " + Translit->fromTranslit(user.split("_")[1]), QIcon(":/icons/pic/icon.png"));
//                }
//            } else {
//                trayIcon->showMessage("Система зачётов", "Новое сообщение от пользователя " + Translit->fromTranslit(user.split("_")[1]), QIcon(":/icons/pic/icon.png"));
//            }
            QString val = file.readAll();
            file.close();
            QString text = data.sliced(QString().number(id).size() + user.size() + lesson.size() + 9);
            QJsonObject message{{"id", id}, {"type", "text_message"}, {"from", user}, {"task", lesson}, {"time", strtime}, {"text", text}};
            file.setFileName(home_path + "/.СистемаЗачётов/.history/chat_" + lesson + ".json");
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            if(val == ""){
                file.write(QJsonDocument(message).toJson(QJsonDocument::Indented));
            } else{
                QJsonDocument messages = QJsonDocument::fromJson(val.toLocal8Bit());
                file.write(QJsonDocument(messages.array() + message).toJson(QJsonDocument::Indented));
            }
            file.close();
            ui->chat_viewer->addItem(text);
        }
    }

}

void MainWindow::sender(){
    if(sock_send->isOpen() && !ui->chat_viewer->isHidden()){
        //************
        // send to server
        //************
        QFile file;
        file.setFileName(home_path + "/.СистемаЗачётов/.history/chat_" + Translit->toTranslit(current_lesson) + "_" + Translit->toTranslit(class_num).replace("_", "") + "@" + Translit->toTranslit(current_excersise).replace(" ", "_") + ".json");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();
        file.close();
        QStringList take_persons;
        take_persons += person_type + "_" + login + "_" + Translit->toTranslit(class_num).replace("_", "");
        if(val != ""){
            QJsonDocument d = QJsonDocument::fromJson(val.toLocal8Bit());
            foreach(QJsonValueRef raw_message, d.array()){
                QJsonObject message = raw_message.toObject();
                if(take_persons.filter(message["from"].toString()).size() == 0 && message["from"].toString() != person_type + "_" + login + "_" + class_num){
                    take_persons += message["from"].toString();
                }
            }
        } else {
            trayIcon->showMessage("Система зачётов", "Временно система не может отправить сообщение", QIcon(":/icons/pic/icon.png"));
            return;
        }
        qWarning() << "[TO:" + take_persons.join("_") + "]&:&" + current_lesson + "_" + Translit->toTranslit(class_num).replace("_", "") + "@" + current_excersise + ":%:" + ui->message->toPlainText().replace('\n', "#$#");
        sock_send->write(QString("[TO:" + take_persons.join(" ") + "]&:&" + Translit->toTranslit(current_lesson) + "_" + Translit->toTranslit(class_num).replace("_", "") + "@" + Translit->toTranslit(current_excersise).replace(" ", "_") + ":%:" + ui->message->toPlainText().replace('\n', "#$#")).toLocal8Bit());
        sock_send->waitForBytesWritten();
        ui->message->clear();
        //************
        // end
        //************
    } else {
        trayIcon->showMessage("Система зачётов", "Нет соединения с сервером сообщений", QIcon(":/icons/pic/icon.png"));
    }
}


void MainWindow::on_reconnect_triggered()
{
    sock_send->write("disconnect");
    sock_send->waitForBytesWritten();
    disconnect(sock, SIGNAL(readyRead()), this, SLOT(reciver()));
    sock_send->close();
    sock->close();
    reconnectingFTP();
    connectToMessageServer();
}


void MainWindow::on_upload_btn_clicked()
{
    if(person_type == "student"){
        if(username != "" && current_needed_files.size() != 0){
            if(ui->chat_viewer->isHidden()){
                QString file_types="";
                for(int i=0; i < current_needed_files.size(); i++){
                    file_types += "*." + current_needed_files[i].split("_")[1] + " ";
                }
                QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Send files"), home_path, tr(QString("Files " + file_types + "").toLocal8Bit()));
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
                QString error_mes = "";
                foreach(QString file, current_needed_files){
                    if(count_fileends[file.split("_")[1]] != file.split("_")[0].toInt()){
                        error_mes += file.split("_")[1] + ": " + QString::number(count_fileends[file.split("_")[1]]) + "/" + file.split("_")[0] + "\n";
                    }
                }
                if(error_mes != ""){
                    QMessageBox::critical(this, "Ошибка отправки задания", "Вы выбрали неверное количество файлов:\n" + error_mes);
                } else{
                    qWarning() << current_lesson;
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
                    int code = ftp->UploadFile(file_path.toStdString(), QString(person_type + "/" + class_num.left(class_num.size() - 1) + "/ready_tasks/" + file_path.split('/').takeLast()).toStdString());
                    if(code == 1){
                        trayIcon->showMessage("Система зачётов", "Задание отправлено на проверку", QIcon(":/icons/pic/icon.png"));
                    } else{
                        QMessageBox::critical(this, "Ошибка добавления задания", "Связь с сервером утеряна, задание не отправлено");
                        //trayIcon->showMessage("Система зачётов", "Задание не было отправлено на проверку, проверьте соединение с сервером", QIcon(":/icons/pic/icon.png"));
                    }
                }
            }
        }
    }
}


void MainWindow::on_add_task_btn_clicked()
{
    if(current_lesson == ""){
        QMessageBox::critical(this, "Ошибка добавления задания", "Вы не выбрали урок. Создайте или выберите урок перед созданием задания");
    } else{
        AddTaskDialog * dialog = new AddTaskDialog();
        QString num_class = current_lesson.split("_").takeLast();
        socket->connectToHost("alexavr.ru", 25555, QTcpSocket::ReadWrite);
        socket->waitForConnected();
        socket->write(QString("com://get_tasks " + encode(num_class, 0)).toLocal8Bit());
        socket->waitForReadyRead();
        QByteArray task_data = socket->readAll();
        socket->close();
        if(task_data == "" || task_data == "Error 404! Server not found!"){
            trayIcon->showMessage("Система зачётов", "Связь с сервером Системы Тестов отсутствует", QIcon(":/icons/pic/icon.png"));
            dialog->remove_task(2);
        } else{
            foreach(QString task, QString(task_data).split("$")){
                dialog->add_task(task.split("|")[1]);
            }
        }
        int ans = dialog->exec();
        if(ans == QDialog::Accepted){
            QStringList task = dialog->get_task();
            delete dialog;
            if(task == QStringList({"", ""})){
                QMessageBox::critical(this, "Ошибка добавления задания", "Вы не выбрали задание");
            } else if(task[1] == ""){
                QMessageBox::critical(this, "Ошибка добавления задания", "Вы не выбрали/заполнили задание");
            } else if(task[0] == "classic"){
                if(task[2] == ""){
                    QMessageBox::critical(this, "Ошибка добавления задания", "Вы не написали название");
                    return;
                } else if(task[3] == ""){
                    QMessageBox::critical(this, "Ошибка добавления задания", "Вы не выбрали типы файлов для выполненных заданий, подробнее написано во вкладке помощь");
                    return;
                }
                QString filename = home_path + "/.СистемаЗачётов/" + Translit->toTranslit(current_lesson) + ".json";
                QJsonObject lesson{{"title", task[2]}, {"description", task[1]}, {"file", task[3].replace(" ", "&")}};
                lessons[current_lesson] = lessons[current_lesson].toArray() + lesson;
                lessons_data = lessons[current_lesson].toArray();
                QFile file;
                file.setFileName(filename);
                file.open(QIODevice::WriteOnly);
                file.write(QJsonDocument(lessons[current_lesson].toArray()).toJson(QJsonDocument::Indented));
                file.close();
                if(ftp->UploadFile(filename.toStdString(), QString("student/" + current_lesson.split("_")[1] + "/tasks/" + filename.split('/').takeLast()).toStdString())){
                    QListWidgetItem *newItem = new QListWidgetItem(ui->excercises);
                    newItem->setTextAlignment(Qt::AlignCenter);
                    newItem->setText(task[2]);
                    newItem->setBackground(QColor(50, 50, 50));
                    newItem->setForeground(QColor("white"));
                    newItem->setFont(QFont("", 14));
                    ui->excercises->addItem(newItem);
                    trayIcon->showMessage("Система зачётов", "Задание отправлено ученикам", QIcon(":/icons/pic/icon.png"));
                } else{
                    QMessageBox::critical(this, "Ошибка добавления задания", "Связь с сервером утеряна, задание не отправлено");
                }
            } else if(task[0] == "tasksystem"){
                int p = current_lesson.split("_").size();
                QString num_class = Translit->toTranslit(QString(current_lesson.split("_")[p - 2] + current_lesson.split("_")[p - 1]).replace("_", ""));
                socket->connectToHost("alexavr.ru", 25555, QTcpSocket::ReadWrite);
                socket->waitForConnected();
                socket->write(QString("com://give_task " + encode(num_class, 0) + "&&" + task[1]).toLocal8Bit());
                socket->waitForReadyRead();
                QByteArray data = socket->readAll();
                qWarning() << data;
                if(data == "Ok"){
                    trayIcon->showMessage("Система зачётов", "Задание отправлено ученикам", QIcon(":/icons/pic/icon.png"));
                } else if(data == ""){
                    QMessageBox::critical(this, "Ошибка добавления задания", "Связь с сервером утеряна, задание не отправлено");
                } else {
                    QMessageBox::critical(this, "Ошибка добавления задания", "Ошибка сервера");
                }
                socket->close();
            }
        } else{
            delete dialog;
        }
    }


}


void MainWindow::on_add_lesson_clicked()
{
   AddLessonDialog * dialog = new AddLessonDialog();
   int ans = dialog->exec();
   if(ans == QDialog::Accepted){
       QStringList new_lesson = dialog->get_lesson();
       if(new_lesson[0] == ""){
           QMessageBox::critical(this, "Ошибка добавления предмета", "Вы не указали название предмета");
       } else if(new_lesson[1].size() > 3 || new_lesson[1].size() < 2){
           QMessageBox::critical(this, "Ошибка добавления предмета", "Вы не указали класс предмета");
       } else{
            QString filename = home_path + "/.СистемаЗачётов/" + Translit->toTranslit(new_lesson[0] + "_" + new_lesson[1].left(new_lesson[1].size() - 1) + "_" + new_lesson[1].right(1)) + ".json";
            QJsonArray temp_tasks;
            lessons[Translit->toTranslit(new_lesson[0] + "_" + new_lesson[1])] = temp_tasks;
            QFile file;
            file.setFileName(filename);
            file.open(QIODevice::WriteOnly);
            file.write(QJsonDocument(temp_tasks).toJson(QJsonDocument::Indented));
            file.close();
            if(ftp->UploadFile(filename.toStdString(), QString("student/" + new_lesson[1].left(new_lesson[1].size() - 1) + "/tasks/" + filename.split('/').takeLast()).toStdString())){
                ui->menu_2->addAction(new_lesson[0] + "_" + new_lesson[1].left(new_lesson[1].size() - 1) + "_" + new_lesson[1].right(1));
            }
       }
   }
   delete dialog;
}



void MainWindow::on_accept_mark_clicked()
{
    socket->connectToHost("alexavr.ru", 25555, QTcpSocket::ReadWrite);
    socket->waitForConnected();

    qWarning() << current_files << ui->ready_tasks_list->currentItem()->text();
    qWarning() << current_files[ui->ready_tasks_list->currentItem()->text()];

    QString task = current_files[ui->ready_tasks_list->currentItem()->text()].split("/").takeLast().split(".").takeFirst();

    qWarning() << task.split("$");
    qWarning() << task.split("$")[1];
    qWarning() << task.split("$")[2] << task.split("$")[2].split("_");

    QString name = task.split("$")[2].right(task.split("$")[2].size() - task.split("$")[2].split("_")[0].size() - 1);
    QString _class = task.split("$")[2].split("_").takeFirst();
    QString mark = Translit->toTranslit(ui->marks->currentText());
    QString excercise = task.split("$")[1];
    QString lesson = task.split("$")[0];
    QDateTime time = QDateTime::currentDateTime();
    QString strtime = time.toString("dd.MM.yyyy&hh:mm:ss");

    QFile file;
    file.setFileName(home_path + "/.СистемаЗачётов/number_" + login);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    long long number = file.readAll().toLongLong();
    file.close();
    qWarning() << QString("com://take_mark " + name + "&&" + _class + "&&" + encode(mark, number) + "&&" + lesson + "&&" + excercise + "&&" + strtime + "&&" + encode(login, number));
    socket->write(QString("com://take_mark " + name + "&&" + _class + "&&" + encode(mark, number) + "&&" + lesson + "&&" + excercise + "&&" + strtime + "&&" + encode(login, number)).toLocal8Bit());
    socket->waitForReadyRead();
    QByteArray data = socket->readAll();
    qWarning() << data;
    socket->close();
}

void MainWindow::on_about_triggered()
{
    about_dialog->exec();
}


void MainWindow::on_remove_cash_triggered()
{
    QDir home(home_path + "/.СистемаЗачётов");
    home.removeRecursively();
    QDir::home().mkpath(home_path + "/.СистемаЗачётов/tmp");
    QDir::home().mkpath(home_path + "/.СистемаЗачётов/.history");
    trayIcon->showMessage("Система зачётов", "Кэш очищен", QIcon(":/icons/pic/icon.png"));
}


void MainWindow::on_help_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/zavrik965/EvaluationSystemQt/wiki/%D0%A1%D0%B8%D1%81%D1%82%D0%B5%D0%BC%D0%B0-%D0%97%D0%B0%D1%87%D1%91%D1%82%D0%BE%D0%B2"));
}

