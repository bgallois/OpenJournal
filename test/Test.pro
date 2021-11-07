QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql svg

TARGET = Test
TEMPLATE = app
CONFIG += testcase
CONFIG += no_testcase_installs

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build

SOURCES += \
        test.cpp \
        ../src/journal.cpp \

QMAKE_CXXFLAGS += -std=c++17 -O3 -fopenmp -g

INCLUDEPATH += /usr/include/gtest
LIBS += -lgtest -lgtest_main
HEADERS += \
        ../src/journal.h \
        /usr/include/gtest/gtest.h \

RESOURCES += \
        resources.qrc\
