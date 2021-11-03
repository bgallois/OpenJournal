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

TRANSLATIONS = \
            ../translations/openjournal_en.ts \
            ../translations/openjournal_fr.ts
qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_VAR_OBJECTS_DIR}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

ICON = ../resources/icon.icns
RC_ICONS = ../resources/icon.ico

unix:!macx {
  TARGET = openjournal

  # custom destination INSTALL_ROOT=$PWD/package_root make install
  target.path = /usr/local/bin/
  desktop.path = /usr/local/share/applications/
  desktop.files = ../io.github.bgallois.openjournal.desktop
  metainfo.path = /usr/local/share/metainfo/
  metainfo.files = ../io.github.bgallois.openjournal.metainfo.xml
  icon.path = /usr/local/share/icons/
  icon.files = resources/openjournal.png
  license.path = /usr/share/local/licenses/openjournal/
  license.files = ../LICENSE
  translation.path = /usr/local/share/openjournal/
  translation.files = $$DESTDIR/openjournal_en.qm $$DESTDIR/openjournal_fr.qm
  translation.CONFIG += no_check_exist
  INSTALLS += target desktop metainfo icon license translation
}

