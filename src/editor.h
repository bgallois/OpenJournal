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

#ifndef EDITOR_H
#define EDITOR_H

#include <QApplication>
#include <QDebug>
#include <QDropEvent>
#include <QElapsedTimer>
#include <QFile>
#include <QKeySequence>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStatusBar>
#include <QToolBar>
#include "qmarkdowntextedit.h"

class Editor : public QMarkdownTextEdit {
  Q_OBJECT
 public:
  explicit Editor(QWidget *parent = 0, bool initHighlighter = true);
  ~Editor();
  bool isBusy;

 private:
  QElapsedTimer *inUse;

 protected:
  int buffer;
  void setBuffer();
  void dropEvent(QDropEvent *dropEvent) override;
 public slots:
  void addToolBarActions(QToolBar *toolBar, QStatusBar *statusBar = nullptr);
  void insertFormattingTemplate(QString balise);
  void insertToDoTemplate();
  void insertTableTemplate();
  void insertLinkTemplate();
  void insertImageTemplate();
  void insertListTemplate();
  void insertListNumberedTemplate();
  void insertFormulaTemplate();
  void forceBufferChange();
  void setBusy(bool isBusy);
  bool isUsed();
 signals:
  void bufferChanged(QString text);
};

#endif  // EDITOR_H
