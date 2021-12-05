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
#include "addalarm.h"
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
  QSqlDatabase db;
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
  void openCloud(QString username, QString password, QUrl endpoint);
  void openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile);
  void loadJournal(const QDate date);
  void clearJournal();
  void saveSettings();
  void backup();
  void refresh();
  void refreshCursor();
  void exportAll();
  void exportCurrent();
  void saveCurrent();
  void importEntry();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void closeEvent(QCloseEvent *event);
  void setTodayReminder(const QString text, QMap<QString, QTimer *> &reminders);
  bool eventFilter(QObject *obj, QEvent *event);
  void about();
  void reboot();
  bool loadStyle(const QString path);
  void addImage(QString path);
  void clearTemporaryFiles();
  QByteArray downloadHttpFile(QUrl url);
  void switchJournalMode(QString mode);

 signals:
  void exportLoadingFinished();
};

#endif  // MAINWINDOW_H
