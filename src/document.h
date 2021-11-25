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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QString>

class Document : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged FINAL)
 public:
  explicit Document(QObject *parent = nullptr) : QObject(parent) {}

  void setText(const QString &text);

 signals:
  void textChanged(const QString &text);

 private:
  QString m_text;
};

#endif  // DOCUMENT_H
