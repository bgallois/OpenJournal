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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QActionGroup>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QIcon>
#include <QInputDialog>
#include <QLCDNumber>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSettings>
#include <QSoundEffect>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringLiteral>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <QWebChannel>
#include "dialog_cloud.h"
#include "document.h"
#include "journal.h"
#include "journal_cloud.h"
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
  QString saveDir;
  QString dbConnection;
  Journal *page;
  JournalCloud *pageCloud;
  Journal *pageLocal;
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
  QStringList tmpFiles;
  int cursorPosition;
  int cursorAnchor;
  int scrollPosition;

 protected:
  void changeEvent(QEvent *event) override;

 private slots:
  void newJournal();
  void newJournal(QString plannerName);
  void openJournal();
  void openJournal(QString plannerName);
  void openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile);
  void loadEntry(const QDate date);
  void refreshEntry();
  void openCloud(QString username, QString password, QUrl endpoint);
  void switchJournalMode(const QString mode);
  void clearJournal();
  void backup();
  void refresh();
  void refreshCursor();
  void exportAll();
  void exportCurrent();
  void saveCurrent();
  void importEntry();
  void saveSettings();
  void loadLanguages();
  bool loadStyle(const QString path);
  void addImage(QString path);
  void clearTemporaryFiles();
  void setTodayReminder(const QString text, QMap<QString, QTimer *> &reminders);
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  bool eventFilter(QObject *obj, QEvent *event);
  void about();
  void reboot();
  QByteArray downloadHttpFile(QUrl url);

 signals:
  void exportLoadingFinished();
  void getEntry(QDate date);
};

#endif  // MAINWINDOW_H
