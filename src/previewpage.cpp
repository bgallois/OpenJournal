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

#include "previewpage.h"

#include <QDesktopServices>

bool PreviewPage::acceptNavigationRequest(const QUrl &url,
                                          QWebEnginePage::NavigationType /*type*/,
                                          bool /*isMainFrame*/) {
  // Only allow qrc:/index.html.
  if (url.scheme() == QString("qrc"))
    return true;
  QDesktopServices::openUrl(url);
  return false;
}
