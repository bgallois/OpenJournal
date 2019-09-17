#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseSoftwareOpenGL);
    MainWindow w;
    w.setWindowIcon(QIcon(":/openjournal.svg"));
    w.show();

    return a.exec();
}
