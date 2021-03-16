#include "journalpage.h"

JournalPage::JournalPage(QSqlDatabase &database, QDate date, QObject *parent) : QObject(parent) {
  db = database;
  setDate(date);
}

JournalPage::~JournalPage() {
}

void JournalPage::setEntry(QString e) {
  entry = e;
  writeToDatabase();
}

void JournalPage::setDate(QDate d) {
  date = d;
}

void JournalPage::writeToDatabase() {
  // Deletes the previous entry for this date
  QSqlQuery query(db);
  query.prepare("DELETE FROM journalPage WHERE date = ? ");
  query.addBindValue(date.toString("yyyy.MM.dd"));
  query.exec();

  // Inserts the new entry
  query.prepare(
      "INSERT INTO journalPage (date, entry) "
      "VALUES (?, ?)");
  query.bindValue(0, date.toString("yyyy.MM.dd"));
  query.bindValue(1, entry);
  query.exec();
}

void JournalPage::readFromDatabase() {
  // Reads from database the entry at date equal current date
  QSqlQuery query(db);
  query.prepare("SELECT * FROM journalPage WHERE date = ?");
  query.addBindValue(date.toString("yyyy.MM.dd"));
  query.exec();
  emit(getDate(date.toString("dddd MMMM d yyyy")));

  if (query.first()) {
    entry = query.value(1).toString();
    emit(getEntry(entry));
  }
}

void JournalPage::readFromDatabaseAll() {
  QSqlQuery query(db);
  query.prepare("SELECT * FROM journalPage");
  query.exec();
  QString data;
  while (query.next()) {
    data.append("# " + query.value(0).toString() + "\n" + query.value(1).toString() + "\n\n");
  }
  emit(getAll(data));
}
