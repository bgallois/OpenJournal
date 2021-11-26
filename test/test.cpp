#include <QDir>
#include <QFile>
#include "../src/journal.h"
#include "gtest/gtest.h"

namespace {
class JournalTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(JournalTest, testConst) {
  QFile::copy(":/test.jnl", QDir::tempPath() + "/test.jnl");
  QFile::setPermissions(QDir::tempPath() + "/test.jnl", QFile::WriteOwner | QFile::ReadOwner);
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(QDir::tempPath() + "/test.jnl");
  if (db.open()) {
    Journal testJournal(db, QDate(2021, 10, 31));
    QString entryRef("test31");
    QString entry;
    QObject::connect(&testJournal, &Journal::getEntry, [&entry](QString data) {
      entry = data;
    });
    testJournal.readFromDatabase();
    EXPECT_EQ(entry, entryRef);

    entryRef = "false";
    testJournal.readFromDatabase();
    EXPECT_NE(entry, entryRef);

    testJournal.setDate(QDate(2021, 10, 30));
    entryRef = "test30";
    testJournal.readFromDatabase();
    EXPECT_EQ(entry, entryRef);

    entryRef = "false";
    testJournal.readFromDatabase();
    EXPECT_NE(entry, entryRef);

    QObject::connect(&testJournal, &Journal::getAll, [&entry](QString data) {
      entry = data;
    });
    testJournal.setDate(QDate(2021, 10, 31));
    testJournal.readFromDatabaseAll();
    entryRef = QString("# 2021.10.29\ntest29\n\n# 2021.10.30\ntest30\n\n# 2021.10.31\ntest31\n\n");
    EXPECT_EQ(entry, entryRef);
  }
  else {
    FAIL();
  }
  QFile::remove(QDir::tempPath() + "/test.jnl");
}

TEST_F(JournalTest, testWrite) {
  QFile::copy(":/test.jnl", QDir::tempPath() + "/test.jnl");
  QFile::setPermissions(QDir::tempPath() + "/test.jnl", QFile::WriteOwner | QFile::ReadOwner);
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(QDir::tempPath() + "/test.jnl");
  if (db.open()) {
    Journal testJournal(db, QDate(2021, 10, 31));
    QString entry;
    QString entryRef;
    QObject::connect(&testJournal, &Journal::getEntry, [&entry](QString data) {
      entry = data;
    });
    // Test write
    entryRef = "test write";
    testJournal.setEntry(entryRef);
    testJournal.readFromDatabase();
    EXPECT_EQ(entry, entryRef);
    testJournal.setEntry("test31");
    // Test read-only
    testJournal.setReadOnly(true);
    testJournal.setEntry(entryRef);
    testJournal.readFromDatabase();
    EXPECT_EQ(entry, "test31");
  }
  else {
    FAIL();
  }
  QFile::remove(QDir::tempPath() + "/test.jnl");
}

TEST_F(JournalTest, active) {
  QFile::copy(":/test.jnl", QDir::tempPath() + "/test.jnl");
  QFile::setPermissions(QDir::tempPath() + "/test.jnl", QFile::WriteOwner | QFile::ReadOwner);
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(QDir::tempPath() + "/test.jnl");
  if (db.open()) {
    Journal testJournal(db, QDate(2021, 10, 31));
    EXPECT_EQ(testJournal.isActive(), true);
    db.close();
    EXPECT_EQ(testJournal.isActive(), false);
  }
  else {
    FAIL();
  }
  QFile::remove(QDir::tempPath() + "/test.jnl");
}

TEST_F(JournalTest, insert_retieveImage) {
  QFile image(":/Lenna.png");
  QByteArray imageData;
  if (image.open(QIODevice::ReadOnly)) {
    imageData = image.readAll();
    image.close();
  }
  else {
    FAIL();
  }
  QFile::copy(":/test.jnl", QDir::tempPath() + "/test.jnl");
  QFile::setPermissions(QDir::tempPath() + "/test.jnl", QFile::WriteOwner | QFile::ReadOwner);
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(QDir::tempPath() + "/test.jnl");
  if (db.open()) {
    Journal testJournal(db, QDate(2021, 10, 31));
    testJournal.insertImage("Lenna.png", imageData);
    EXPECT_EQ(imageData, testJournal.retrieveImage("Lenna.png"));
    EXPECT_EQ(QByteArray(), testJournal.retrieveImage("enna.png"));
  }
  else {
    FAIL();
  }
  QFile::remove(QDir::tempPath() + "/test.jnl");
}

TEST_F(JournalTest, cleanImage) {
  QFile image(":/Lenna.png");
  QByteArray imageData;
  if (image.open(QIODevice::ReadOnly)) {
    imageData = image.readAll();
    image.close();
  }
  else {
    FAIL();
  }
  QFile::copy(":/test.jnl", QDir::tempPath() + "/test.jnl");
  QFile::setPermissions(QDir::tempPath() + "/test.jnl", QFile::WriteOwner | QFile::ReadOwner);
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(QDir::tempPath() + "/test.jnl");
  if (db.open()) {
    Journal testJournal(db, QDate(2021, 10, 31));
    testJournal.insertImage("Lenna.png", imageData);
    EXPECT_EQ(imageData, testJournal.retrieveImage("Lenna.png"));
    testJournal.setEntry("test31");
    testJournal.clearUnusedImages();
    EXPECT_EQ(QByteArray(), testJournal.retrieveImage("Lenna.png"));
  }
  else {
    FAIL();
  }
  QFile::remove(QDir::tempPath() + "/test.jnl");
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
