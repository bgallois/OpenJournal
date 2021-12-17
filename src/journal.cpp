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
  isEnable = true;
}

Journal::~Journal() {
}

/**
 * Request an entry to be written in the journal.
 */
void Journal::setEntry(QString e) {
  if (isEnable && (entry != e)) {
    entry = e;
    writeToDatabase();
  }
}

/**
 * Request an entry to be read from the journal.
 */
void Journal::requestEntry(QDate date) {
  setDate(date);
  readFromDatabase();
}

/**
 * Close the current journal.
 */
void Journal::close() {
  db = QSqlDatabase();
}

/**
 * Set the journal by opening a database.
 */
void Journal::setDatabase(QSqlDatabase &database, bool isReadOnly) {
  db = database;
  QSqlQuery query(db);
  query.prepare("CREATE TABLE IF NOT EXISTS asset ( filename TEXT, imagedata LONGBLOB )");
  query.exec();
  query.prepare("CREATE TABLE IF NOT EXISTS journalPage (date TINYTEXT, entry TEXT)");
  query.exec();
  // Create a unique index to replace entry
  // Driver dependente command
  if (db.driverName() == "QMYSQL" || db.driverName() == "QMARIADB") {
    query.prepare("ALTER TABLE journalPage ADD CONSTRAINT date_un UNIQUE (date(255))");
    query.exec();
  }
  else if (db.driverName() == "QSQLITE") {
    query.prepare("CREATE UNIQUE INDEX date_un ON journalPage(date);");
    query.exec();
  }
  setReadOnly(isReadOnly);
}

/**
 * Set the current date.
 */
void Journal::setDate(QDate d) {
  date = d;
}

/**
 * Set journal as read-only.
 */
void Journal::setReadOnly(bool isLocked) {
  isReadOnly = isLocked;
}

/**
 * Private function to write in the database the current entry.
 */
void Journal::writeToDatabase() {
  if (isReadOnly) {
    return;
  }

  QSqlQuery query(db);
  if (entry.isEmpty()) {
    query.prepare("DELETE FROM journalPage WHERE date = ? ");
    query.addBindValue(date.toString("yyyy.MM.dd"));
    query.exec();
  }
  else {
    if (db.driverName() == "QMYSQL" || db.driverName() == "QMARIADB") {
      query.prepare(
          "INSERT INTO journalPage (date, entry) "
          "VALUES (?, ?) ON DUPLICATE KEY UPDATE entry = ? ");
      query.bindValue(0, date.toString("yyyy.MM.dd"));
      query.bindValue(1, entry);
      query.bindValue(2, entry);
      query.exec();
    }
    else if (db.driverName() == "QSQLITE") {
      query.prepare(
          "INSERT OR REPLACE INTO journalPage (date, entry) "
          "VALUES (?, ?)");
      query.bindValue(0, date.toString("yyyy.MM.dd"));
      query.bindValue(1, entry);
      query.exec();
    }
  }
}

/**
 * Private function to read from the database at the current date.
 */
void Journal::readFromDatabase() {
  // Reads from database the entry at date equal current date
  QSqlQuery query(db);
  query.prepare("SELECT entry FROM journalPage WHERE date = ?");
  query.addBindValue(date.toString("yyyy.MM.dd"));
  query.exec();
  emit(getDate(date.toString(Qt::DefaultLocaleLongDate)));

  if (query.first()) {
    entry = query.value(0).toString();
    emit(getEntry(entry));
  }
  else {
    emit(getEntry(QString()));
  }
}

/**
 * Read all the entry in the database.
 */
void Journal::readFromDatabaseAll() {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM journalPage ORDER BY date");
  query.exec();
  QString data;
  while (query.next()) {
    data.append("# " + query.value(0).toString() + "\n" + query.value(1).toString() + "\n\n");
  }
  emit(getAll(data));
}

/**
 * Insert an image in the database.
 */
void Journal::insertImage(QString name, QByteArray imageData) {
  QSqlQuery query(db);
  query.prepare(
      "INSERT INTO asset (filename, imagedata)"
      "VALUES (?, ?)");
  query.bindValue(0, name);
  query.bindValue(1, imageData);
  query.exec();
}

/**
 * Retrieve an image form the database.
 */
void Journal::retrieveImage(QString name, QString path) {
  QSqlQuery query(db);
  query.prepare("SELECT imagedata FROM asset WHERE filename = ?");
  query.addBindValue(name);
  query.exec();
  QByteArray imageData;
  if (query.first()) {
    imageData = query.value(0).toByteArray();
  }
  emit(getImage(imageData, path));
}

/**
 * Delete images that are not referenced in any entry.
 */
void Journal::clearUnusedImages() {
  QSqlQuery query(db);
  query.prepare("SELECT entry FROM journalPage");
  query.exec();
  QString data;
  while (query.next()) {
    data.append(query.value(0).toString());
  }

  query.prepare("SELECT filename FROM asset");
  query.exec();
  while (query.next()) {
    if (!data.contains(query.value(0).toString())) {
      QSqlQuery delQuery(db);
      delQuery.prepare("DELETE FROM asset WHERE filename = ?");
      delQuery.addBindValue(query.value(0).toString());
      delQuery.exec();
    }
  }
}

/**
 * Check if the database connection is active.
 */
bool Journal::isActive() {
  QSqlQuery query(db);
  query.prepare("SELECT 1");
  return query.exec();
}

/**
 * Set the journal enable.
 */
void Journal::setEnabled(bool isEnable) {
  this->isEnable = isEnable;
}
