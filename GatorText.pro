#-------------------------------------------------
#
# Project created by QtCreator 2018-03-25T22:48:33
#
#-------------------------------------------------

QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GatorText
TEMPLATE = app

QT += webenginewidgets
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
        main.cpp \
    gatortext.cpp \
    codeeditor.cpp \
    highlighter.cpp \
    findreplacedialog.cpp \
    changefontdialog.cpp \
    changefontsizedialog.cpp

HEADERS += \
        mainwindow.h \
    gatortext.h \
    codeeditor.h \
    highlighter.h \
    languagetypes.h \
    findreplacedialog.h \
    changefontdialog.h \
    changefontsizedialog.h

FORMS += \
        mainwindow.ui \
    findreplacedialog.ui \
    changefontdialog.ui \
    changefontsizedialog.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
