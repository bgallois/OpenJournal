#include <QApplication>
#include <QDir>
#include <QFile>
#include "../src/editor.h"
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

namespace {
class EditorTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(EditorTest, formatting) {
  Editor editor;
  editor.setText("Test test");
  QTextCursor cursor = editor.textCursor();
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertFormattingTemplate("*");
  EXPECT_EQ(editor.toPlainText(), "*Test* test");
  EXPECT_EQ(editor.textCursor().position(), 6);

  editor.setText("Test test");
  cursor = editor.textCursor();
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertFormattingTemplate("**");
  EXPECT_EQ(editor.toPlainText(), "**Test** test");
  EXPECT_EQ(editor.textCursor().position(), 8);

  editor.setText("Test test");
  cursor = editor.textCursor();
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertFormattingTemplate("*");
  EXPECT_EQ(editor.toPlainText(), "Test** test");
  EXPECT_EQ(editor.textCursor().position(), 5);
}

TEST_F(EditorTest, Todo) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 9);
  editor.setTextCursor(cursor);
  editor.insertToDoTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test test\n\n### To do list\n\n- [ ] Task_1");
  EXPECT_EQ(editor.textCursor().selectedText(), "Task_1");
  EXPECT_EQ(editor.textCursor().position(), 33);
}

TEST_F(EditorTest, Table) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 9);
  editor.setTextCursor(cursor);
  editor.insertTableTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test test\n\nA | B | C\n--- | --- | ---\nA1 | B1 | C1\nA2 | B2 | C2\nA3 | B3 | C3\n\n");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), QString("\n\nA | B | C\n--- | --- | ---\nA1 | B1 | C1\nA2 | B2 | C2\nA3 | B3 | C3\n\n").size() + 9);
}

TEST_F(EditorTest, Link) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("http test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertLinkTemplate();
  EXPECT_EQ(editor.toPlainText(), "[](http) test");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 1);

  cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertLinkTemplate();
  EXPECT_EQ(editor.toPlainText(), "[Test](http://) test");
  EXPECT_EQ(editor.textCursor().selectedText(), "http://");
  EXPECT_EQ(editor.textCursor().position(), 7);

  cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertLinkTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test[](http://) test");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 5);
}

TEST_F(EditorTest, List) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::End);
  editor.setTextCursor(cursor);
  editor.insertListTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test test\n\n* Item_1");
  EXPECT_EQ(editor.textCursor().selectedText(), "Item_1");
  EXPECT_EQ(editor.textCursor().position(), QString("\n\n* ").size() + 9);

  cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::End);
  editor.setTextCursor(cursor);
  editor.insertListNumberedTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test test\n\n1. Item_1");
  EXPECT_EQ(editor.textCursor().selectedText(), "Item_1");
  EXPECT_EQ(editor.textCursor().position(), QString("\n\n* .").size() + 9);
}

TEST_F(EditorTest, Formula) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertFormulaTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test\\\\[  \\\\] test");  // C++ string needs double escaping
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 8);

  cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertFormulaTemplate();
  EXPECT_EQ(editor.toPlainText(), "\\\\[ Test \\\\] test");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 12);
}

TEST_F(EditorTest, Image) {
  Editor editor;
  QTextCursor cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertImageTemplate();
  EXPECT_EQ(editor.toPlainText(), "![](Test) test");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 8);

  cursor = editor.textCursor();
  editor.setText("Test test");
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 4);
  editor.setTextCursor(cursor);
  editor.insertImageTemplate();
  EXPECT_EQ(editor.toPlainText(), "Test![](http://) test");
  EXPECT_EQ(editor.textCursor().selectedText(), "");
  EXPECT_EQ(editor.textCursor().position(), 6);
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  QApplication app(argc, argv);
  return RUN_ALL_TESTS();
}
