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

#include "editor.h"

Editor::Editor(QWidget *parent, bool initHighlighter) : QMarkdownTextEdit(parent, initHighlighter) {
  connect(this, &QPlainTextEdit::textChanged, this, &Editor::setBuffer);
}

void Editor::setBuffer() {
  if (buffer++; buffer > 20) {
    emit(bufferChanged(this->toPlainText()));
    buffer = 0;
  }
}

void Editor::forceBufferChange() {
  emit(bufferChanged(this->toPlainText()));
  buffer = 0;
}

void Editor::dropEvent(QDropEvent *dropEvent) {
  // Add functionnalities
  const QMimeData *mimeData = dropEvent->mimeData();
  if (mimeData->hasUrls()) {
    QList<QUrl> urls = mimeData->urls();
    for (const auto &a : urls) {
      QMimeDatabase db;
      QMimeType mime = db.mimeTypeForFile(a.toString());
      if (mime.name().contains("image/")) {  // Generic image/* not supported on qt?
        this->insertPlainText(QString("![](%1)\n").arg(a.toString()));
      }
      else if (mime.inherits("text/plain")) {
        QFile file(a.path());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          this->insertPlainText(QString(file.readAll()));
        }
      }
    }
  }

  // Call base class dropEvent
  // This is a workaround to call the base class dropEvent without adding url to text.
  // If Base::dropEvent is not called, the cursor is not redraw.
  setReadOnly(true);
  QPlainTextEdit::dropEvent(dropEvent);
  setReadOnly(false);
}

void Editor::addToolBarActions(QToolBar *toolBar, QStatusBar *statusBar) {
  QKeySequence keySequence = QKeySequence(Qt::CTRL + Qt::Key_B);
  QAction *actionBoldTemplate = new QAction(QIcon(":/bold.png"), tr("Bold ") + keySequence.toString(), this);
  actionBoldTemplate->setShortcut(keySequence);
  connect(actionBoldTemplate, &QAction::triggered, [this]() {
    insertFormattingTemplate("**");
  });
  toolBar->addAction(actionBoldTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_I);
  QAction *actionItalicTemplate = new QAction(QIcon(":/italic.png"), tr("Italic ") + keySequence.toString(), this);
  actionItalicTemplate->setShortcut(keySequence);
  connect(actionItalicTemplate, &QAction::triggered, [this]() {
    insertFormattingTemplate("*");
  });
  toolBar->addAction(actionItalicTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_S);
  QAction *actionStrikeTemplate = new QAction(QIcon(":/strikethrough.png"), tr("Strikethrough ") + keySequence.toString(), this);
  actionStrikeTemplate->setShortcut(keySequence);
  connect(actionStrikeTemplate, &QAction::triggered, [this]() {
    insertFormattingTemplate("~~");
  });
  toolBar->addAction(actionStrikeTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_D);
  QAction *actionToDoTemplate = new QAction(QIcon(":/todo.png"), tr("To do list template ") + keySequence.toString(), this);
  actionToDoTemplate->setShortcut(keySequence);
  connect(actionToDoTemplate, &QAction::triggered, this, &Editor::insertToDoTemplate);
  toolBar->addAction(actionToDoTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_L);
  QAction *actionListNumberedTemplate = new QAction(QIcon(":/listNumbered.png"), tr("Numbered list ") + keySequence.toString(), this);
  actionListNumberedTemplate->setShortcut(keySequence);
  connect(actionListNumberedTemplate, &QAction::triggered, this, &Editor::insertListNumberedTemplate);
  toolBar->addAction(actionListNumberedTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L);
  QAction *actionListTemplate = new QAction(QIcon(":/list.png"), tr("Unordered list ") + keySequence.toString(), this);
  actionListTemplate->setShortcut(keySequence);
  connect(actionListTemplate, &QAction::triggered, this, &Editor::insertListTemplate);
  toolBar->addAction(actionListTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_T);
  QAction *actionTableTemplate = new QAction(QIcon(":/table.png"), tr("Table template ") + keySequence.toString(), this);
  actionTableTemplate->setShortcut(keySequence);
  connect(actionTableTemplate, &QAction::triggered, this, &Editor::insertTableTemplate);
  toolBar->addAction(actionTableTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_U);
  QAction *actionLinkTemplate = new QAction(QIcon(":/link.png"), tr("URL template ") + keySequence.toString(), this);
  actionLinkTemplate->setShortcut(keySequence);
  connect(actionLinkTemplate, &QAction::triggered, this, &Editor::insertLinkTemplate);
  toolBar->addAction(actionLinkTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I);
  QAction *actionImageTemplate = new QAction(QIcon(":/image.png"), tr("Image template ") + keySequence.toString(), this);
  actionImageTemplate->setShortcut(keySequence);
  connect(actionImageTemplate, &QAction::triggered, this, &Editor::insertImageTemplate);
  toolBar->addAction(actionImageTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_F);
  QAction *actionFormulaTemplate = new QAction(QIcon(":/formula.png"), tr("Formula template ") + keySequence.toString(), this);
  actionFormulaTemplate->setShortcut(keySequence);
  connect(actionFormulaTemplate, &QAction::triggered, this, &Editor::insertFormulaTemplate);
  toolBar->addAction(actionFormulaTemplate);
  toolBar->addSeparator();

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_Escape);
  QAction *actionLock = new QAction(QIcon(":/lock.png"), tr("Lock journal ") + keySequence.toString(), this);
  actionLock->setShortcut(keySequence);
  actionLock->setCheckable(true);
  actionLock->setChecked(false);
  connect(actionLock, &QAction::triggered, this, &QMarkdownTextEdit::setDisabled);
  connect(actionLock, &QAction::triggered, [this, statusBar](bool status) {
    if (status) {
      statusBar->showMessage(tr("The journal is locked"));
    }
    else {
      statusBar->showMessage(tr("The journal is unlocked"), 2000);
    }
  });
  toolBar->addAction(actionLock);
}

void Editor::insertFormattingTemplate(QString balise) {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    if (cursor.hasSelection()) {
      this->insertPlainText(balise + cursor.selectedText() + balise);
    }
    else {
      this->insertPlainText(balise + cursor.selectedText() + balise);
      cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, balise.size());
    }
    this->setTextCursor(cursor);
  }
}

void Editor::insertToDoTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    this->insertPlainText("\n\n### To do list\n\n- [ ] Task_1");
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
  }
}

void Editor::insertTableTemplate() {
  if (this->isEnabled()) {
    this->insertPlainText("\n\nA | B | C\n--- | --- | ---\nA1 | B1 | C1\nA2 | B2 | C2\nA3 | B3 | C3\n\n");
  }
}

void Editor::insertLinkTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    if (cursor.hasSelection()) {
      if (cursor.selectedText().contains("http")) {
        int selectionSize = cursor.selectedText().size();
        this->insertPlainText(QString("[](%1)").arg(cursor.selectedText()));
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, selectionSize + 3);
      }
      else {
        this->insertPlainText(QString("[%1](http://)").arg(cursor.selectedText()));
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 7);
      }
    }
    else {
      this->insertPlainText("[](http://)");
      cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 10);
    }
    this->setTextCursor(cursor);
  }
}

void Editor::insertImageTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    if (cursor.hasSelection()) {
      this->insertPlainText(QString("![](%1)").arg(cursor.selectedText()));
      cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
    }
    else {
      this->insertPlainText("![](http://)");
      cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 10);
    }
    this->setTextCursor(cursor);
  }
}

void Editor::insertListNumberedTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    this->insertPlainText("\n\n1. Item_1");
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
  }
}

void Editor::insertListTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    this->insertPlainText("\n\n* Item_1");
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
  }
}

void Editor::insertFormulaTemplate() {
  if (this->isEnabled()) {
    QTextCursor cursor = this->textCursor();
    if (cursor.hasSelection()) {
      this->insertPlainText(QString("\\\\[ %1 \\\\]").arg(cursor.selectedText()));
    }
    else {
      this->insertPlainText("\\\\[  \\\\]");
      cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 4);
    }
    this->setTextCursor(cursor);
  }
}
