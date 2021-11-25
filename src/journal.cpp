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

#include "journal.h"

Journal::Journal(QSqlDatabase &database, QDate date, bool isReadOnly, QObject *parent) : QObject(parent) {
  setDatabase(database);
  setDate(date);
  setReadOnly(isReadOnly);
}

Journal::~Journal() {
}

void Journal::setEntry(QString e) {
  entry = e;
  writeToDatabase();
}

void Journal::setDatabase(QSqlDatabase &database, bool isReadOnly) {
  db = database;
  setReadOnly(isReadOnly);
}

void Journal::setDate(QDate d) {
  date = d;
}

void Journal::setReadOnly(bool isLocked) {
  isReadOnly = isLocked;
}

void Journal::writeToDatabase() {
  if (isReadOnly) {
    return;
  }
  // Deletes the previous entry for this date
  QSqlQuery query(db);
  query.prepare("DELETE FROM journalPage WHERE date = ? ");
  query.addBindValue(date.toString("yyyy.MM.dd"));
  query.exec();

  // Inserts the new entry
  if (!entry.isEmpty()) {
    query.prepare(
        "INSERT INTO journalPage (date, entry) "
        "VALUES (?, ?)");
    query.bindValue(0, date.toString("yyyy.MM.dd"));
    query.bindValue(1, entry);
    query.exec();
  }
}

void Journal::readFromDatabase() {
  // Reads from database the entry at date equal current date
  QSqlQuery query(db);
  query.prepare("SELECT * FROM journalPage WHERE date = ?");
  query.addBindValue(date.toString("yyyy.MM.dd"));
  query.exec();
  emit(getDate(date.toString(Qt::DefaultLocaleLongDate)));

  if (query.first()) {
    entry = query.value(1).toString();
    emit(getEntry(entry));
  }
}

void Journal::readFromDatabaseAll() {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM journalPage");
  query.exec();
  QString data;
  while (query.next()) {
    data.append("# " + query.value(0).toString() + "\n" + query.value(1).toString() + "\n\n");
  }
  emit(getAll(data));
}

bool Journal::isActive() {
  QSqlQuery query(db);
  query.prepare("SELECT 1");
  return query.exec();
}
