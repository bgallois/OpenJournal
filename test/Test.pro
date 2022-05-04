QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql svg

TARGET = Test
TEMPLATE = app
CONFIG += testcase
CONFIG += no_testcase_installs
CONFIG += c++17

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

unix:!macx {
    INCLUDEPATH += /usr/include/gtest
    LIBS += -lgtest -lgtest_main
}

unix:macx {
  QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@executable_path/../Frameworks/
  QT_CONFIG -= no-pkg-config
  CONFIG  += link_pkgconfig
  PKGCONFIG += gtest
}

win32 {
  QMAKE_CFLAGS_RELEASE += /MT
  QMAKE_CXXFLAGS_RELEASE += /MT
  QMAKE_CFLAGS_RELEASE -= -MD
  QMAKE_CXXFLAGS_RELEASE -= -MD
  LIBS += -L"$$PWD/../googletest/build/install/lib" -lgtest -lgtest_main
  INCLUDEPATH += "$$PWD/../googletest/build/install/include/"
}

SOURCES += \
        test.cpp \
        ../src/journal.cpp \
        ../src/qmarkdowntextedit.cpp \
        ../src/markdownhighlighter.cpp \
        ../src/editor.cpp \
        ../src/qplaintexteditsearchwidget.cpp \

FORMS += \
        ../src/qplaintexteditsearchwidget.ui \

HEADERS += \
        ../src/journal.h \
        ../src/qmarkdowntextedit.h \
        ../src/markdownhighlighter.h \
        ../src/editor.h \
        ../src/qplaintexteditsearchwidget.h \

RESOURCES += \
        resources.qrc\
