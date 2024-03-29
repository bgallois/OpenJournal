#-------------------------------------------------
#
# Project created by QtCreator 2019-05-18T12:17:12
#
#-------------------------------------------------

QT       += core gui sql webenginewidgets webchannel multimedia
QTPLUGIN += QSQLMYSQL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

TARGET = OpenJournal
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS += -std=c++11 -O3 -g
CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        journalpage.cpp \
        qmarkdowntextedit.cpp \
        markdownhighlighter.cpp \
        qplaintexteditsearchwidget.cpp \
        previewpage.cpp \
        document.cpp \
        addalarm.cpp \

HEADERS += \
        mainwindow.h \
        journalpage.h \
        qmarkdowntextedit.h \
        markdownhighlighter.h \
        qplaintexteditsearchwidget.h \
        previewpage.h \
        document.h \
        addalarm.h \

FORMS += \
        mainwindow.ui\
        qplaintexteditsearchwidget.ui \
        addalarm.ui \

RESOURCES = \
            ../resources/openjournal.qrc

ICON = ../resources/icon.icns
RC_ICONS = ../resources/icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
