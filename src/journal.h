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
  void getImage(QByteArray image, QString path);
  void getAll(QString data);
  void getDate(QString date);

 public slots:
  void setEntry(QString entry);
  void setDate(QDate date);
  void setDatabase(QSqlDatabase &databasebool, bool isReadOnly = false);
  void setReadOnly(bool isLocked);
  bool isActive();
  void insertImage(QString name, QByteArray imageData);
  void clearUnusedImages();
  void retrieveImage(QString name, QString path);
};

#endif
