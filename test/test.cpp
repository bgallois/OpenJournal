#include "../src/journalpage.h"
#include "gtest/gtest.h"

namespace {
class JournalPageTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(JournalPageTest, testConst) {
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(":/test.jnl");
  if (db.open()) {
    JournalPage testJournal(db, QDate(2021, 10, 31));
    QString entryRef("test31");
    QString entry;
    QObject::connect(&testJournal, &JournalPage::getEntry, [&entry](QString data) {
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

    QObject::connect(&testJournal, &JournalPage::getAll, [&entry](QString data) {
      entry = data;
    });
    testJournal.setDate(QDate(2021, 10, 31));
    testJournal.readFromDatabaseAll();
    entryRef = QString("# 2021.10.29\ntest29\n\n# 2021.10.30\ntest30\n\n# 2021.10.31\ntest31\n\n");
    EXPECT_EQ(entry, entryRef);
  }
}

TEST_F(JournalPageTest, testWrite) {
  QSqlDatabase db;
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(":/test.jnl");
  if (db.open()) {
    JournalPage testJournal(db, QDate(2021, 10, 31));
    QString entry;
    QString entryRef;
    QObject::connect(&testJournal, &JournalPage::getEntry, [&entry](QString data) {
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
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
