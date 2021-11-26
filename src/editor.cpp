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

void Editor::dropEvent(QDropEvent *dropEvent) {
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
}
