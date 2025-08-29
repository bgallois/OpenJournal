#!/bin/bash

if [ "$1" = "release" ]; then
  set -e
  qmake6 CONFIG+=released QMAKE_CXXFLAGS+=-DENABLE_DEVTOOL QMAKE_CXXFLAGS+=-s OpenJournal.pro
  make
  make check
  src/build/openjournal
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake6 CONFIG+=debug OpenJournal.pro
  make
  make check
  src/build/openjournal
fi

if [ "$1" = "profile" ]; then
  set -e
  qmake6 QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug OpenJournal.pro
  make
  src/build/openjournal
  gprof build/FastTrack build/gmon.out > profiling.txt
fi
