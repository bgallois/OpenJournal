#include <signal.h>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
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
  QString path = QDir::tempPath() + "/OpenJournal.lock";
  QFile lock(path);
  if (lock.exists()) {
    QMessageBox::information(nullptr, "An instance of OpenJournal is already running", "An instance of OpenJournal is already running.\nIf it is not the case delete" + path + " and restart OpenJournal");
    return 1;
  }

  lock.open(QIODevice::WriteOnly | QIODevice::Text);
  MainWindow w;
  w.setWindowIcon(QIcon(":/openjournal.svg"));
  w.show();
  qApp->setApplicationVersion("0.0.1");
  QFontDatabase::addApplicationFont(":/Lato.ttf");
  QFontDatabase::addApplicationFont(":/Caveat.ttf");
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");
  QObject::connect(&a, &QApplication::aboutToQuit, &lock, [&lock]() {
    lock.remove();
  });

  return a.exec();
}
