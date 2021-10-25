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
  JournalPage() = default;
  JournalPage(QSqlDatabase &database, QDate date, bool isReadOnly = false, QObject *parent = nullptr);
  JournalPage(const JournalPage &) = delete;
  JournalPage &operator=(const JournalPage &) = delete;
  ~JournalPage();

 private:
  QString entry;
  QSqlDatabase db;
  QDate date;
  bool isReadOnly;

 public:
  void readFromDatabase();
  void readFromDatabaseAll();
  void writeToDatabase();

 signals:
  void getEntry(QString entry);
  void getAll(QString data);
  void getDate(QString date);

 public slots:
  void setEntry(QString entry);
  void setDate(QDate date);
  void setDatabase(QSqlDatabase &databasebool, bool isReadOnly = false);
  void setReadOnly(bool isLocked);
};

#endif
