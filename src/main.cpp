#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setAttribute(Qt::AA_UseSoftwareOpenGL);
  MainWindow w;
  w.setWindowIcon(QIcon(":/openjournal.svg"));
  w.show();
  qApp->setApplicationVersion("0.0.1");

  return a.exec();
}
