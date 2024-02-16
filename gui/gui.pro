TEMPLATE = app
TARGET = gui
INCLUDEPATH += .

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
QT += widgets
QT += charts
SOURCES += mainwindow.cpp
HEADERS += mainwindow.h
