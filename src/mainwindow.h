#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include <QDebug>
#include <QIcon>
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QWebChannel>
#include <QTimer>
#include <QStringLiteral>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QMap>
#include "journalpage.h"
#include "previewpage.h"
#include "document.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  QSqlDatabase db;
  JournalPage *page;
  QString plannerName;
  PreviewPage *previewPage;
  Document doc;
  Ui::MainWindow *ui;
  QTimer *refreshTimer;
  QSystemTrayIcon *trayIcon;
  QStringList *reminders;

 private slots:
  void newJournal();
  void newJournal(QString plannerName);
  void openJournal();
  void openJournal(QString plannerName);
  void loadJournalPage(const QDate date);
  void clearJournalPage();
  void saveSettings();
  void backup();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  void reminder(QString text, QStringList *reminders);
};

#endif  // MAINWINDOW_H
