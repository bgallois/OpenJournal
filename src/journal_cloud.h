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

#ifndef JOURNAL_CLOUD_H
#define JOURNAL_CLOUD_H

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include "journal.h"

class JournalCloud : public Journal {
  Q_OBJECT
 public:
  JournalCloud();
  JournalCloud(QUrl url, QString username, QString password, bool isReadOnly = false);
  ~JournalCloud();
  void setDatabase(QUrl url, QString user, QString pass, bool readOnly = false);
  QString username;

 private:
  void readFromDatabase();
  void readFromDatabaseAll();
  void writeToDatabase();

 private slots:
  void httpReply(QNetworkReply *reply);
  void httpRequest(QString type, QString data = QString(), QString date = QString(), QString content = QString());

 public slots:
  void insertImage(QString name, QByteArray imageData);
  void clearUnusedImages();
  void retrieveImage(QString name, QString path);
  bool isActive();
  void close();

 private:
  QUrl endpoint;
  QString password;
  QNetworkAccessManager *manager;

 signals:
  void networkStatus(QString errorCode);
};

#endif  // EDITOR_H
