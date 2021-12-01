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

#include "journal_cloud.h"

JournalCloud::JournalCloud() {
  manager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this, &JournalCloud::httpReply);
}

JournalCloud::JournalCloud(QUrl url, QString username, QString password, bool isReadOnly) : JournalCloud() {
  setDatabase(url, username, password, isReadOnly);
  setDate(date);
  setReadOnly(isReadOnly);
}

void JournalCloud::close() {
  endpoint = QUrl();
  username = QString();
  password = QString();
}

void JournalCloud::setDatabase(QUrl url, QString user, QString pass, bool readOnly) {
  endpoint = url;
  username = user;
  password = pass;
  setReadOnly(readOnly);
}

JournalCloud::~JournalCloud() {
  delete manager;
}

void JournalCloud::writeToDatabase() {
  if (isReadOnly) {
    return;
  }
  httpRequest("insert", "text", date.toString("yyyy.MM.dd"), entry);
}

void JournalCloud::readFromDatabase() {
  httpRequest("query", "text", date.toString("yyyy.MM.dd"));
  emit(getDate(date.toString(Qt::DefaultLocaleLongDate)));
}

void JournalCloud::readFromDatabaseAll() {
  httpRequest("query_all");
}

void JournalCloud::insertImage(QString name, QByteArray imageData) {
  httpRequest("insert", "image", name, imageData.toHex());
}

void JournalCloud::retrieveImage(QString name, QString path) {
  httpRequest("query", "image", name, path);
}

void JournalCloud::clearUnusedImages() {
  httpRequest("clean");
}

bool JournalCloud::isActive() {
  // Always active because send crendential at each request
  return true;
}

void JournalCloud::httpReply(QNetworkReply *reply) {
  QByteArray replyString = reply->readAll();
  if (reply->error() == QNetworkReply::ConnectionRefusedError || replyString.isEmpty() || replyString == "500") {
    emit(networkStatus(false));
    return;
  }
  emit(networkStatus(true));
  QJsonDocument doc = QJsonDocument::fromJson(replyString);
  QJsonObject replyData = doc.object();
  if (replyData["type"].toString() == "query") {
    if (replyData["data"].toString() == "text") {
      emit(getEntry(replyData["content"].toString()));
    }
    else if (replyData["data"].toString() == "image") {
      emit(getImage(QByteArray::fromHex(replyData["content"].toString().toUtf8()), replyData["identifier"].toString()));
    }
  }
  reply->deleteLater();
}

void JournalCloud::httpRequest(QString type, QString data, QString date, QString content) {
  QUrlQuery postData;
  postData.addQueryItem("user", username);
  postData.addQueryItem("type", type);
  postData.addQueryItem("password", password);
  postData.addQueryItem("journal", "journal");
  postData.addQueryItem("data", data);
  postData.addQueryItem("identifier", date);
  postData.addQueryItem("content", content);
  QNetworkRequest request(endpoint);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  manager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
}
