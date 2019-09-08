#ifndef JOURNALPAGE_H
#define JOURNALPAGE_H

#include <QDate>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

class JournalPage : public QObject {
  Q_OBJECT
 public:
  JournalPage(QSqlDatabase &database, QDate date, QObject *parent = nullptr);
  ~JournalPage();

 private:
  QString entry;
  QSqlDatabase db;
  QDate date;

 public:
  void readFromDatabase();
  void writeToDatabase();

 signals:
  void getEntry(QString entry);
  void getDate(QString date);

 public slots:
  void setEntry(QString entry);
  void setDate(QDate date);
};

#endif
