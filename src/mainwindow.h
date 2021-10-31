#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QGraphicsBlurEffect>
#include <QIcon>
#include <QInputDialog>
#include <QLCDNumber>
#include <QMap>
#include <QMessageBox>
#include <QRegularExpression>
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
  QMap<QString, QTimer *> reminders;
  bool isPrivate;
  bool isSonore;
  QSoundEffect *alarmSound;
  QSettings *settings;
  QLabel *statusMessage;
  QLCDNumber *clock;
  bool isHelp;

 private slots:
  void newJournal();
  void newJournal(QString plannerName);
  void openJournal();
  void openJournal(QString plannerName);
  void openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile);
  void loadJournalPage(const QDate date);
  void clearJournalPage();
  void saveSettings();
  void backup();
  void refresh();
  void exportAll();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  void setTodayReminder(const QString text, QMap<QString, QTimer *> &reminders);
  bool eventFilter(QObject *obj, QEvent *event);
  void about();
  void insertToDoTemplate();

 signals:
  void exportLoadingFinished();
};

#endif  // MAINWINDOW_H
