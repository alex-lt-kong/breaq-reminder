#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T22:15:07
#
#-------------------------------------------------

QT += core gui multimedia widgets

TARGET = QBreakReminder
TEMPLATE = app

#QMAKE_LFLAGS = -no-pie
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        dialog-editnotes.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        dialog-editnotes.h \
        mainwindow.h

FORMS += \
        dialog-editnotes.ui \
        mainwindow.ui

RC_ICONS = Leaf.ico

#CONFIG += sanitizer sanitize_memory

RESOURCES += \
    res.qrc

VERSION = 1.1
