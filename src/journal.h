#ifndef JOURNAL_H
#define JOURNAL_H

#include <QDate>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

class Journal : public QObject {
  Q_OBJECT
 public:
  Journal() = default;
  Journal(QSqlDatabase &database, QDate date, bool isReadOnly = false, QObject *parent = nullptr);
  Journal(const Journal &) = delete;
  Journal &operator=(const Journal &) = delete;
  ~Journal();

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
  bool isActive();
};

#endif
