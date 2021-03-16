#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QIcon>
#include <QMap>
#include <QMessageBox>
#include <QSettings>
#include <QSoundEffect>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringLiteral>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QUrl>
#include <QWebChannel>
#include "addalarm.h"
#include "document.h"
#include "journalpage.h"
#include "previewpage.h"

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
  bool isPrivate;
  bool isSonore;
  QSoundEffect *alarmSound;
  QSettings *settings;

 private slots:
  void newJournal();
  void newJournal(QString plannerName);
  void openJournal();
  void openJournal(QString plannerName);
  void loadJournalPage(const QDate date);
  void clearJournalPage();
  void saveSettings();
  void backup();
  void exportAll();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  void reminder(QString text, QStringList *reminders);
  bool eventFilter(QObject *obj, QEvent *event);

 signals:
  void exportLoadingFinished();
};

#endif  // MAINWINDOW_H
