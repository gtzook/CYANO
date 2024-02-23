TEMPLATE = app
TARGET = gui
INCLUDEPATH += .

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
QT += widgets
QT += charts
QT += network
SOURCES += mainwindow.cpp
SOURCES += settings_window/settings.cpp
HEADERS += mainwindow.h
HEADERS += settings_window/settings.h
