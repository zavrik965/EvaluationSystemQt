QT       += core gui network gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -L/usr/local/lib -lftp
INCLUDEPATH += -L/usr/local/include
SOURCES += \
    add_lesson_dialog.cpp \
    add_task_dialog.cpp \
    coder.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    translitter.cpp

HEADERS += \
    add_lesson_dialog.h \
    add_task_dialog.h \
    logindialog.h \
    mainwindow.h \
    translitter.h

FORMS += \
    add_lesson_dialog.ui \
    add_task_dialog.ui \
    logindialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
