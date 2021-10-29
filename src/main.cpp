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
    QMessageBox::information(nullptr, "An instance of OpenJournal is already running", "An instance of OpenJournal is already running.\nIf it is not the case delete " + path + " and restart OpenJournal");
    return 1;
  }

  lock.open(QIODevice::WriteOnly | QIODevice::Text);
  a.setOrganizationName("Analysabl");
  a.setApplicationName("OpenJournal");
  a.setApplicationVersion("1.0.0");
  QFontDatabase::addApplicationFont(":/Lato.ttf");
  QFontDatabase::addApplicationFont(":/Caveat.ttf");
  QObject::connect(&a, &QApplication::aboutToQuit, &lock, [&lock]() {
    lock.remove();
  });
  MainWindow w;
  w.setWindowIcon(QIcon(":/openjournal.svg"));
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");
  w.show();

  return a.exec();
}
