QT       += core gui sql webenginewidgets webchannel multimedia
QTPLUGIN += QSQLMYSQL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql svg

TEMPLATE = app

win32 {
  TARGET = OpenJournal
}

unix:macx {
  TARGET = OpenJournal
}

unix:!macx {
  TARGET = openjournal

  # custom destination INSTALL_ROOT=$PWD/package_root make install
  target.path = /usr/bin/
  desktop.path = /usr/share/applications/
  desktop.files = io.github.bgallois.openjournal.desktop
  metainfo.path = /usr/share/metainfo/
  metainfo.files = io.github.bgallois.openjournal.metainfo.xml
  icon.path = /usr/share/icons/
  icon.files = resources/openjournal.png
  license.path = /usr/share/licenses/openjournal/
  license.files = LICENSE
  INSTALLS += target desktop metainfo icon license
}

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
