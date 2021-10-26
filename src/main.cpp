#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowIcon(QIcon(":/openjournal.svg"));
  w.show();
  qApp->setApplicationVersion("0.0.1");
  QFontDatabase::addApplicationFont(":/Lato.ttf");
  QFontDatabase::addApplicationFont(":/Caveat.ttf");
  w.setStyleSheet("QWidget {font-family: 'Lato', sans-serif;}");

  return a.exec();
}
