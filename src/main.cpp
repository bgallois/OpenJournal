/*
Copyright (C) 2021 Benjamin Gallois benjamin at gallois.cc

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <signal.h>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QLockFile>
#include <QMessageBox>
#include "mainwindow.h"

void handleSigint(int sig) {
  qApp->quit();
}
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  // Handle SIGINT signal and perform clean up
  signal(SIGINT, handleSigint);

  // Check if application is already running
  QLockFile lock(QDir::tempPath() + "/OpenJournal_.lock");
  if (!lock.tryLock(100)) {
    QMessageBox::information(nullptr, "An instance of OpenJournal is already running", "An instance of OpenJournal is already running.");
    return 1;
  }

  a.setOrganizationName("Analyzable");
  a.setApplicationName("OpenJournal");
  a.setApplicationVersion("1.3.4");
  QFontDatabase::addApplicationFont(":/Lato.ttf");
  QFontDatabase::addApplicationFont(":/Caveat.ttf");
  MainWindow w;
  w.setWindowIcon(QIcon(":/openjournal.svg"));
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");
  w.show();

  return a.exec();
}
