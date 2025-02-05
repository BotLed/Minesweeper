TEMPLATE = app

QT += core widgets  # Add QtWidgets module
SOURCES += ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/qrightclickbutton.cpp \
    ../src/popup.cpp

HEADERS += ../include/mainwindow.h \
    ../include/qrightclickbutton.h \
    ../include/popup.h

RESOURCES += ../include/resources/resources.qrc

CONFIG += console

OBJECTS_DIR = ../src

DESTDIR = ../

INCLUDEPATH += ../include
