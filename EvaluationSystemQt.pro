QT       += core gui network gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -lcurl
SOURCES += \
    CurlHandle.cpp \
    FTPClient.cpp \
    aboutdialog.cpp \
    addlessondialog.cpp \
    addtaskdialog.cpp \
    coder.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    preferencedialog.cpp \
    translitter.cpp

HEADERS += \
    CurlHandle.h \
    FTPClient.h \
    aboutdialog.h \
    addlessondialog.h \
    addtaskdialog.h \
    logindialog.h \
    mainwindow.h \
    preferencedialog.h \
    translitter.h

FORMS += \
    aboutdialog.ui \
    addlessondialog.ui \
    addtaskdialog.ui \
    logindialog.ui \
    mainwindow.ui \
    preferencedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=
