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

#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWebEnginePage>

class PreviewPage : public QWebEnginePage {
  Q_OBJECT
 public:
  explicit PreviewPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}

 protected:
  bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif  // PREVIEWPAGE_H
