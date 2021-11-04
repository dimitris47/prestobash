QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = prestobash
TEMPLATE = app
VERSION = 0.1
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    commanddialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    commanddialog.h \
    global.h \
    mainwindow.h

FORMS += \
    commanddialog.ui \
    mainwindow.ui

unix:!android {
    target.path = $${PREFIX}/bin
    icon.path = /usr/share/pixmaps
    icon.files = icons/prestobash.png
    desktop.path = /usr/share/applications
    desktop.files = prestobash.desktop
    INSTALLS += target icon desktop
}

RESOURCES += \
    prestobash.qrc
