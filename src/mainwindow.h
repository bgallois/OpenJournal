#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include <QAction>
#include <QActionGroup>
#include <QDebug>
#include <QDesktopServices>
#include <QGraphicsBlurEffect>
#include <QIcon>
#include <QInputDialog>
#include <QLCDNumber>
#include <QMap>
#include <QMessageBox>
#include <QProcess>
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
#include <QTranslator>
#include <QUrl>
#include <QWebChannel>
#include "addalarm.h"
#include "document.h"
#include "journal.h"
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
  QString lang;
  QString style;
  QSqlDatabase db;
  Journal *page;
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
  QTranslator translator;

 protected:
  void changeEvent(QEvent *event) override;

 private slots:
  void newJournal();
  void newJournal(QString plannerName);
  void openJournal();
  void openJournal(QString plannerName);
  void openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile);
  void loadJournal(const QDate date);
  void clearJournal();
  void saveSettings();
  void backup();
  void refresh();
  void exportAll();
  void exportCurrent();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  void setTodayReminder(const QString text, QMap<QString, QTimer *> &reminders);
  bool eventFilter(QObject *obj, QEvent *event);
  void about();
  void insertToDoTemplate();
  void insertTableTemplate();
  void insertLinkTemplate();
  void insertListTemplate();
  void insertListNumberedTemplate();
  void reboot();
  bool loadStyle(const QString path);

 signals:
  void exportLoadingFinished();
};

#endif  // MAINWINDOW_H
