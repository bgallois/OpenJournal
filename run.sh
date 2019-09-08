#!/bin/bash

if [ "$1" = "released" ]; then
  set -e
  qmake CONFIG+=released QMAKE_CXXFLAHE+=-DENABLE_DEVTOOL src/OpenJournal.pro
  make clean
  make
  cd build
  ./OpenJournal
  cd ..
  make clean
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake CONFIG+=debug src/OpenJournal.pro
  make
  cd build
  ./OpenJournal
  cd ..
fi

if [ "$1" = "profile" ]; then
  set -e
  qmake QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug src/OpenJournal.pro
  make
  cd build
  ./OpenJournal
  cd ..
  gprof build/FastTrack build/gmon.out > profiling.txt
fi
