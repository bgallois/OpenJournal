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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  // ui set up
  ui->setupUi(this);
  QFontDatabase::addApplicationFont(":/Caveat.ttf");
  ui->date->setStyleSheet("QLabel{font: 21pt 'Caveat';}");
  statusMessage = new QLabel();
  ui->statusBar->addWidget(statusMessage);
  clock = new QLCDNumber();
  clock->setSegmentStyle(QLCDNumber::Flat);
  ui->statusBar->addPermanentWidget(clock);
  isHelp = true;  // Will display help message the first time app is closed

  // Window geometry
  settings = new QSettings(this);
  this->resize(settings->value("mainwindow/size", QSize(400, 400)).toSize());
  this->move(settings->value("mainwindow/pos", QPoint(200, 200)).toPoint());

  // Splitters geometry
  ui->splitter->setSizes(settings->value("mainwindow/splitter", QVariant::fromValue(QList<int>{619, 350})).value<QList<int>>());
  ui->splitter_2->setSizes(settings->value("mainwindow/splitter_2", QVariant::fromValue(QList<int>{194, 483})).value<QList<int>>());

  // Translation
  this->loadLanguages();

  // Setup style
  style = settings->value("settings/style", "openjournal").toString();
  loadStyle(":/" + style + ".qss");

  QStringList styles{"openjournal", "qt", "crema", "macos"};  // List of available themes in resource
  QActionGroup *styleGroup = new QActionGroup(this);
  for (const auto &a : styles) {  // For each theme create and connect the action in the style menu
    QAction *styleAction = new QAction(a, this);
    styleGroup->addAction(styleAction);
    styleAction->setCheckable(true);
    if (a == style) {
      styleAction->setChecked(true);
    }
    connect(styleAction, &QAction::triggered, [this, styleAction]() {
      style = styleAction->text();
      if (style == "qt") {
        qApp->setStyleSheet(styleSheet());
      }
      else {
        loadStyle(":/" + style + ".qss");
      }
    });
    ui->menuStyle->addAction(styleAction);
  }

  // Tray integration
  trayIcon = new QSystemTrayIcon(QIcon(":/openjournal.svg"), this);
  QMenu *trayMenu = new QMenu;
  QAction *restore = new QAction(tr("Restore"));
  connect(restore, &QAction::triggered, [this]() {
    this->showNormal();
    ui->calendar->setSelectedDate(QDate::currentDate());
    refreshTimer->start();
    refresh();
  });
  trayMenu->addAction(restore);
  QAction *close = new QAction(tr("Quit"));
  connect(close, &QAction::triggered, [this]() {
    saveSettings();
    clearTemporaryFiles();
    page->clearUnusedImages();
    qApp->quit();
  });
  trayMenu->addAction(close);
  connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  // Preview
  previewPage = new PreviewPage(this);
  ui->preview->setPage(previewPage);
  connect(ui->entry, &QPlainTextEdit::textChanged, [this]() {
    this->doc.setText(ui->entry->toPlainText());
    this->setTodayReminder(ui->entry->toPlainText(), reminders);
    this->addImage(ui->entry->toPlainText());
  });
  QWebChannel *channel = new QWebChannel(this);
  channel->registerObject(QStringLiteral("content"), &doc);
  previewPage->setWebChannel(channel);
  ui->preview->setUrl(QUrl("qrc:/index.html"));

  // Connect menu bar
  connect(ui->actionNew_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::newJournal));
  connect(ui->actionOpen_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::openJournal));
  connect(ui->actionConnect_planner, &QAction::triggered, [this]() {
    clearJournal();
    bool isOk;
    QString pass;
    QString info = QInputDialog::getText(this, tr("Connect to a remote server."),
                                         tr("Connect to a remote (local) database. This is an advanced feature see the manual for help.\ndatabaseusername@hostname:port"), QLineEdit::Normal,
                                         settings->value("settings/host", "username@hostname:port").toString(), &isOk);
    if (isOk) {
      pass = QInputDialog::getText(this, tr("JournalName@Password"),
                                   tr("journal@password \n If journal does not exist it will be created and protected by password."), QLineEdit::Normal,
                                   settings->value("settings/remote", "journalname").toString() + "@password", &isOk);
    }
    else {
      statusMessage->setText(tr("No journal is opened"));
    }
    if (isOk && !info.isEmpty() && info.contains(QRegularExpression("\\w+@.+:\\d+")) && info.contains("@")) {
      QStringList tmp = info.split("@");
      QString username = tmp[0];
      QStringList host = tmp[1].split(":");
      QStringList journal = pass.split("@");
      settings->setValue("settings/host", info);
      settings->setValue("settings/remote", journal[0]);
      openJournal(host[0], host[1], username, journal[1], journal[0]);
    }
    else {
      statusMessage->setText(tr("No journal is opened"));
    }
  });
  connect(ui->actionCloud, &QAction::triggered, [this]() {
    clearJournal();
    ui->entry->setEnabled(false);
    DialogCloud dialog(this);
    dialog.setValue(settings->value("settings/cloud", "username").toString());
    if (dialog.exec() == QDialog::Accepted) {
      QPair<QString, QString> val = dialog.getValues();
      openCloud(val.first, val.second, QUrl("https://openjournal.gallois.cc"));
      settings->setValue("settings/cloud", val.first);
      statusMessage->setText(plannerName + tr(" cloud is opened"));
      ui->entry->setEnabled(true);
    }
    else {
      statusMessage->setText(tr("No journal is opened"));
    }
  });
  connect(ui->actionBackup, &QAction::triggered, this, &MainWindow::backup);
  connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);
  connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
  connect(ui->actionSource, &QAction::triggered, [this]() {
    QDesktopServices::openUrl(QUrl("https://github.com/bgallois/OpenJournal", QUrl::TolerantMode));
  });
  connect(ui->actionReport, &QAction::triggered, [this]() {
    QDesktopServices::openUrl(QUrl("https://github.com/bgallois/OpenJournal/issues/", QUrl::TolerantMode));
  });
  connect(ui->actionDocumentation, &QAction::triggered, [this]() {
    QDesktopServices::openUrl(QUrl("https://openjournal.readthedocs.io/en/latest/", QUrl::TolerantMode));
  });
  connect(ui->actionSource, &QAction::triggered, [this]() {
    QDesktopServices::openUrl(QUrl("https://github.com/bgallois/OpenJournal", QUrl::TolerantMode));
  });

  // Connect calendar
  connect(ui->calendar, &QCalendarWidget::selectionChanged, [this]() {
    loadJournal(ui->calendar->selectedDate());
  });

  // Journal page
  pageLocal = new Journal();
  connect(ui->entry, &Editor::bufferChanged, pageLocal, &Journal::setEntry);
  connect(pageLocal, &Journal::getDate, ui->date, &QLabel::setText);
  connect(pageLocal, &Journal::getEntry, ui->entry, &QPlainTextEdit::setPlainText);
  connect(pageLocal, &Journal::getEntry, this, &MainWindow::refreshCursor);
  connect(pageLocal, &Journal::getImage, [this](QByteArray imageData, QString imagePath) {
    QFile image(imagePath);
    if (image.open(QIODevice::WriteOnly)) {
      image.write(imageData);
      tmpFiles.append(imagePath);
      image.close();
    }
  });
  pageCloud = new JournalCloud();
  connect(ui->entry, &Editor::bufferChanged, pageCloud, &JournalCloud::setEntry);
  connect(pageCloud, &JournalCloud::getDate, ui->date, &QLabel::setText);
  connect(pageCloud, &JournalCloud::getEntry, ui->entry, &QPlainTextEdit::setPlainText);
  connect(pageCloud, &JournalCloud::networkStatus, [this](QString error) {
    if (error == "SUCCESS") {
      statusMessage->setText(tr("Your connected to the cloud as ") + pageCloud->username);
    }
    else if (error == "401") {
      statusMessage->setText(tr("The cloud can be reached! Wrong credentials."));
      ui->entry->setEnabled(false);
    }
    else {
      statusMessage->setText(tr("The cloud can be reached! No internet connection. Trying with username: ") + pageCloud->username);
      ui->entry->setEnabled(false);
    }
  });
  connect(pageCloud, &Journal::getEntry, this, &MainWindow::refreshCursor);
  connect(pageCloud, &JournalCloud::getImage, [this](QByteArray imageData, QString imagePath) {
    QFile image(imagePath);
    if (image.open(QIODevice::WriteOnly)) {
      image.write(imageData);
      tmpFiles.append(imagePath);
      image.close();
    }
  });
  switchJournalMode("local");

  // Reads journal settings
  QString lastJournal = settings->value("mainwindow/lastJournal").toString();
  if (QFile(lastJournal).exists()) {
    openJournal(lastJournal);
  }
  else if (QFile(QDir::homePath() + "/OpenJournal.jnl").exists()) {
    openJournal(QDir::homePath() + "/OpenJournal.jnl");
  }
  else {
    newJournal(QDir::homePath() + "/OpenJournal.jnl");
  }

  // Toolbar
  QKeySequence keySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A);
  QAction *actionAddAlarm = new QAction(QIcon(":/clocks.png"), tr("Add an alarm ") + keySequence.toString(), this);
  actionAddAlarm->setShortcut(keySequence);
  connect(actionAddAlarm, &QAction::triggered, [this]() {
    ui->entry->moveCursor(QTextCursor::End);
    AddAlarm *alarm = new AddAlarm();
    connect(alarm, &AddAlarm::alarm, ui->entry, &QMarkdownTextEdit::appendPlainText);
    alarm->exec();
    delete alarm;
  });
  ui->toolBar->addAction(actionAddAlarm);
  ui->toolBar->addSeparator();
  ui->entry->addToolBarActions(ui->toolBar, ui->statusBar);
  addToolBar(Qt::LeftToolBarArea, ui->toolBar);

  // Planner exportation
  connect(ui->actionExport_planner, &QAction::triggered, page, &Journal::readFromDatabaseAll);
  connect(page, &Journal::getAll, [this](QString data) {
    setEnabled(false);
    refreshTimer->stop();
    page->setReadOnly(true);
    doc.setText(data);
    emit(exportLoadingFinished());
  });
  connect(this, &MainWindow::exportLoadingFinished, this, &MainWindow::exportAll);

  // Current entry exportation
  connect(ui->actionExport_current, &QAction::triggered, this, &MainWindow::exportCurrent);
  connect(ui->actionSave_current, &QAction::triggered, this, &MainWindow::saveCurrent);
  connect(ui->actionImport_entry, &QAction::triggered, this, &MainWindow::importEntry);
  saveDir = QDir::homePath();

  // Automatic refresh every 10 seconds
  refreshTimer = new QTimer(this);
  connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refresh);
  refreshTimer->start(10000);

  // Automatic today focus every hour
  QTimer *todayTimer = new QTimer(this);
  connect(
      todayTimer, &QTimer::timeout, todayTimer, [this]() {
        ui->calendar->setSelectedDate(QDate::currentDate());
        loadJournal(ui->calendar->selectedDate());
      });
  todayTimer->start(36000000);

  // Privacy
  isPrivate = settings->value("settings/privacy").toBool();
  QAction *isPrivateAction = new QAction(tr("Private mode"));
  isPrivateAction->setCheckable(true);
  isPrivateAction->setChecked(isPrivate);
  connect(isPrivateAction, &QAction::triggered, [this](bool state) {
    this->setGraphicsEffect(nullptr);
    isPrivate = state;
  });
  ui->menuOptions->addAction(isPrivateAction);

  // Alarm for notification
  alarmSound = new QSoundEffect(this);
  alarmSound->setSource(QUrl("qrc:/notification_0.wav"));
  alarmSound->setLoopCount(20);
  alarmSound->setVolume(0.70);
  connect(trayIcon, &QSystemTrayIcon::messageClicked, alarmSound, &QSoundEffect::stop);
  connect(trayIcon, &QSystemTrayIcon::activated, alarmSound, &QSoundEffect::stop);

  // Sonore
  isSonore = settings->value("settings/sonore").toBool();
  QAction *isSonoreAction = new QAction(tr("Alarm Sound"));
  isSonoreAction->setCheckable(true);
  isSonoreAction->setChecked(isSonore);
  connect(isSonoreAction, &QAction::triggered, [this](bool state) {
    isSonore = state;
  });
  ui->menuOptions->addAction(isSonoreAction);
  this->installEventFilter(this);
}

MainWindow::~MainWindow() {
  delete ui;
  delete page;
}

//////////////// Journal creation, loading and dialog /////////////////////////////////

/**
 * Open a QFileDialog to select a path where to create a new journal.
 * Call the journal creation method.
 */
void MainWindow::newJournal() {
  clearJournal();
  plannerName = QFileDialog::getSaveFileName(this, tr("Save new journal"), "", tr("Journal (*.jnl)"));
  if (plannerName.isEmpty()) return;
  // Adds the correct extension if user forgot
  if (plannerName.right(4) != ".jnl") {
    plannerName.append(".jnl");
  }
  newJournal(plannerName);
}

/**
 * Create an new journal at location fileName.
 */
void MainWindow::newJournal(QString fileName) {
  switchJournalMode("local");
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(fileName);
  ui->entry->setEnabled(false);
  if (!db.open()) {
    plannerName = fileName;
    statusMessage->setText(plannerName + tr(" journal cannot be created"));
    return;
  }
  else {
    ui->statusBar->showMessage(plannerName + tr(" journal is opened"));
    plannerName = plannerName;
    ui->entry->setEnabled(true);
    ui->actionBackup->setEnabled(true);
    ui->calendar->setSelectedDate(QDate::currentDate());
    page->setDatabase(db);
    loadJournal(QDate::currentDate());
  }
}

/**
 * Open a QFileDialog to select a path where to find a journal.
 * Call the journal opening method.
 */
void MainWindow::openJournal() {
  clearJournal();
  ui->entry->setEnabled(false);
  plannerName = QFileDialog::getOpenFileName(this, tr("Open journal"), "", tr("Journal (*.jnl)"));
  if (!plannerName.isEmpty()) {
    openJournal(plannerName);
  }
  else {
    statusMessage->setText(tr("No journal is opened"));
  }
}

/**
 * Open a journal at location plannerFile.
 */
void MainWindow::openJournal(QString plannerFile) {
  switchJournalMode("local");
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(plannerFile);
  if (!db.open()) {
    statusMessage->setText(plannerFile + tr(" journal failed to open"));
    return;
  }
  else {
    plannerName = plannerFile;
    ui->calendar->setSelectedDate(QDate::currentDate());
    page->setDatabase(db);
    loadJournal(QDate::currentDate());
    statusMessage->setText(plannerName + tr(" journal is opened"));
    ui->actionBackup->setEnabled(true);
    ui->entry->setEnabled(true);
  }
}

/**
 * Open a remote journal.
 */
void MainWindow::openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile) {
  switchJournalMode("local");
  clearJournal();
  ui->entry->setEnabled(false);
  db = QSqlDatabase::addDatabase("QMARIADB");
  db.setHostName(hostname);
  db.setPort(port.toInt());
  db.setUserName(username);
  db.setPassword(password);
  if (!db.open()) {  // Check authentification
    statusMessage->setText(tr("Authentification failed"));
    return;
  }
  db.setDatabaseName(plannerFile);
  if (!db.open()) {  // Create database if not exist
    db.setDatabaseName("mysql");
    db.open();
    QSqlQuery query(db);
    query.prepare(QString("CREATE DATABASE %1 ").arg(plannerFile));  // Normaly cannot used place holder for database and table name beware sql injection
    query.exec();
    db.setDatabaseName(plannerFile);
  }
  if (db.open()) {  // If database was successfully created
    plannerName = plannerFile;
    ui->actionBackup->setEnabled(false);
    statusMessage->setText(plannerName + tr(" journal is opened"));
    ui->entry->setEnabled(true);
    ui->calendar->setSelectedDate(QDate::currentDate());
    page->setDatabase(db);
    loadJournal(QDate::currentDate());
  }
  else {
    statusMessage->setText(tr("No journal is opened"));
  }
}

/**
 * Open a QFileDialog to enter the OpenJournal cloud credentials.
 * Connect to the cloud and switch OpenJournal in remote mode.
 */
void MainWindow::openCloud(QString username, QString password, QUrl endpoint) {
  switchJournalMode("cloud");
  ui->calendar->setSelectedDate(QDate::currentDate());
  page->setDatabase(endpoint, username, password);
  loadJournal(QDate::currentDate());
  ui->actionBackup->setEnabled(false);
}

/**
 * Load a journal at a given date.
 */
void MainWindow::loadJournal(const QDate date) {
  if (page->isActive()) {
    ui->entry->forceBufferChange();
    page->setReadOnly(true);  // Prevent erasing old entry
    ui->entry->clear();
    page->setReadOnly(false);
    page->setDate(date);
    page->readFromDatabase();
  }
  else {
    statusMessage->setText(tr("No journal is opened"));
    ui->entry->setEnabled(false);
    ui->entry->clear();
  }
}

/**
 * Clear and close the current journal.
 */
void MainWindow::clearJournal() {
  page->writeToDatabase();
  page->close();
  db.close();
  ui->date->clear();
  ui->entry->clear();
  clearTemporaryFiles();
}

/**
 * Perform a backup of the currently opened journal.
 */
void MainWindow::backup() {
  QFile file(plannerName);
  QFile copy(plannerName + ".back");
  if (!copy.exists()) {
    file.copy(plannerName + ".back");
    ui->statusBar->showMessage(tr("Journal was backed up ") + plannerName + ".back");
  }
  else if (copy.exists()) {
    copy.remove();
    file.copy(plannerName + ".back");
    ui->statusBar->showMessage(tr("Cannot back up journal"));
  }
}

/**
 * Switch OpenJournal mode to the appropriate backend (local or cloud).
 */
void MainWindow::switchJournalMode(const QString mode) {
  if (mode == "local") {
    page = pageLocal;
    pageCloud->blockSignals(true);
    pageLocal->blockSignals(false);
    pageCloud->setEnabled(false);
    pageLocal->setEnabled(true);
  }
  else if (mode == "cloud") {
    page = pageCloud;
    pageLocal->blockSignals(true);
    pageCloud->blockSignals(false);
    pageLocal->setEnabled(false);
    pageCloud->setEnabled(true);
  }
}

//////////////// Journal and entry exportation and importation /////////////////////////////////

/**
 * Export the current journal in PDF.
 */
void MainWindow::exportAll() {
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Select file"), saveDir, tr("Pdf Files (*.pdf)"));
  if (!fileName.isEmpty()) {
    ui->preview->page()->printToPdf(fileName, QPageLayout(QPageSize(ui->preview->page()->contentsSize().toSize()), QPageLayout::Portrait, QMarginsF(10, 20, 10, 20)));
    saveDir = fileName;
  }
  setEnabled(true);
  refreshTimer->start();
  refresh();
  page->setReadOnly(false);
}

/**
 * Export the current journal entry in PDF.
 */
void MainWindow::exportCurrent() {
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save file"), saveDir, tr("Pdf Files (*.pdf)"));
  refreshTimer->stop();
  page->setReadOnly(true);
  if (!fileName.isEmpty()) {
    ui->preview->page()->printToPdf(fileName, QPageLayout(QPageSize(ui->preview->page()->contentsSize().toSize()), QPageLayout::Portrait, QMarginsF(10, 20, 10, 20)));
    saveDir = fileName;
  }
  refreshTimer->start();
  refresh();
  page->setReadOnly(false);
}

/**
 * Export the current journal entry in Markdown.
 */
void MainWindow::saveCurrent() {
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Select file"), saveDir, tr("Markdown Files (*.md)"));
  refreshTimer->stop();
  page->setReadOnly(true);
  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream stream(&file);
    stream << ui->entry->toPlainText();
    file.close();
    saveDir = fileName;
  }
  refreshTimer->start();
  refresh();
  page->setReadOnly(false);
}

/**
 * Append a Markdown file inside the current entry.
 */
void MainWindow::importEntry() {
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open file"), saveDir, tr("Markdown Files (*.md)"));
  refreshTimer->stop();
  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    ui->entry->appendPlainText(in.readAll());
    file.close();
    saveDir = fileName;
  }
  refreshTimer->start();
  refresh();
}

//////////////// MainWindow refresh /////////////////////////////////

/**
 * Refresh the current entry.
 * Keep the current scroll and cursor position and selection.
 */
void MainWindow::refresh() {
  // Triggered the refresh. It will be completed by refreshCursor when setEntry signal
  // is triggered when data has finished loading.
  // Account for the network delay.
  cursorPosition = ui->entry->textCursor().position();
  cursorAnchor = ui->entry->textCursor().anchor();
  scrollPosition = ui->entry->verticalScrollBar()->sliderPosition();
  loadJournal(ui->calendar->selectedDate());
  QTime time = QTime::currentTime();
  clock->display(time.toString("hh:mm"));
}

/**
 * Refresh the cursor.
 * Called after text refresh triggered by the refresh method to take into account the network
 * delay in cloud mode.
 */
void MainWindow::refreshCursor() {
  // Refresh after entry completely downloaded
  QTextCursor cursor = ui->entry->textCursor();
  cursor.setPosition(cursorAnchor);
  cursor.setPosition(cursorPosition, QTextCursor::KeepAnchor);
  ui->entry->setTextCursor(cursor);
  ui->entry->verticalScrollBar()->setSliderPosition(scrollPosition);
}

//////////////// Smart properties, smart image insertion, alarms /////////////////////////////////

/**
 * Add an image inside journal database and change its reference inside the text.
 * At display, the image will be saved in /tmp/ folder and deleted when unecessary or at closing.
 */
void MainWindow::addImage(QString text) {
  bool isTextChanged = false;
  int textSize = text.size();

  // Match all the image Markdown balises ![something](void or something)
  QRegularExpression re("!\\[(.*)\\]\\((.+)\\)");
  QRegularExpressionMatchIterator matches = re.globalMatch(text);
  while (matches.hasNext()) {
    QRegularExpressionMatch match = matches.next();
    QString imageLabel = match.captured(1);
    QString imagePath = match.captured(2);

    // If image just added, add it in database and change reference in text
    if (!imageLabel.contains("OpenJournal_Local_")) {
      QByteArray imageData;
      if (imagePath.contains("http")) {
        imageData = downloadHttpFile(QUrl(imagePath));
      }
      else if (QFile::exists(imagePath.remove("file://"))) {
        QFile image(imagePath);
        if (image.open(QIODevice::ReadOnly)) {
          imageData = image.readAll();
          image.close();
        }
      }
      if (!imageData.isEmpty()) {
        QString extension = QFileInfo(imagePath).suffix();
        imageLabel += QString::number(QRandomGenerator::global()->generate());
        imagePath = "file://" + QDir::tempPath() + "/" + imageLabel + "." + extension;
        imageLabel.insert(0, "OpenJournal_Local_");
        text.replace(match.captured(0), QString("![%1](%2)").arg(imageLabel, imagePath));
        isTextChanged = true;
        page->insertImage(imageLabel, imageData);
      }
    }

    // Save image in local temporary files
    imagePath.remove("file://");
    if (!tmpFiles.contains(imagePath) && imageLabel.contains("OpenJournal_Local_")) {
      page->retrieveImage(imageLabel, imagePath);
    }
  }

  // Update the text if changed
  if (isTextChanged) {
    {
      const QSignalBlocker blocker(ui->entry);
      int cursorPosition = ui->entry->textCursor().position();
      int offset = text.size() - textSize;
      ui->entry->setPlainText(text);
      QTextCursor cursor = ui->entry->textCursor();
      cursor.setPosition(cursorPosition + offset);
      ui->entry->setTextCursor(cursor);
    }
  }
}

/**
 * Clear temporary files necessary to the rendering like images.
 */
void MainWindow::clearTemporaryFiles() {
  for (const auto &a : tmpFiles) {
    QFile::remove(a);
  }
  tmpFiles.clear();
}

/**
 * Synchronous downloading of a remote file.
 */
QByteArray MainWindow::downloadHttpFile(QUrl url) {
  QByteArray downloadedData;
  QNetworkAccessManager *manager = new QNetworkAccessManager(this);
  QEventLoop eventLoop;
  connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
  QNetworkReply *reply = manager->get(QNetworkRequest(url));
  eventLoop.exec();
  downloadedData = reply->readAll();
  if (reply->error() != QNetworkReply::NoError) {
    return QByteArray();
  }
  reply->deleteLater();
  delete manager;
  return downloadedData;
}

/**
 * Set an alarm with the given message at the given time.
 */
void MainWindow::setTodayReminder(const QString text, QMap<QString, QTimer *> &reminders) {
  if (QDate::currentDate() != ui->calendar->selectedDate()) {
    return;
  }
  int end = 0;
  while (end >= 0) {
    int start = text.indexOf("setAlarm(", end);
    if (start != -1) {
      end = text.indexOf(");", start);
      if (end != -1) {
        QStringRef command = text.midRef(start + 9, end - start - 9);
        // Planner exportation
        QVector<QStringRef> commands = command.split(',', QString::SkipEmptyParts);
        if (commands.length() == 2 && !reminders.contains(command.toString())) {
          QVector<QStringRef> timeSet = commands[0].split(':');
          int time = -QTime(timeSet[0].toDouble(), timeSet[1].toDouble()).msecsTo(QTime::currentTime());
          if (time > 0) {
            QTimer *notification = new QTimer(this);
            notification->setSingleShot(true);
            notification->setInterval(time);
            QString message = commands[1].toString();
            connect(notification, &QTimer::timeout, [this, message]() {
              trayIcon->showMessage(tr("Notification"), message, QSystemTrayIcon::Information, 214483648);
              if (isSonore) {
                alarmSound->play();
              }
            });
            notification->start();
            reminders.insert(command.toString(), notification);
          }
        }
      }
    }
    else {
      break;
    }
  }
}

//////////////// OpenJournal settings /////////////////////////////////

/**
 * Save OpenJournal settings.
 */
void MainWindow::saveSettings() {
  settings->setValue("mainwindow/lastJournal", plannerName);
  settings->setValue("mainwindow/size", this->size());
  settings->setValue("mainwindow/pos", this->pos());
  settings->setValue("mainwindow/splitter", QVariant::fromValue(ui->splitter->sizes()));
  settings->setValue("mainwindow/splitter_2", QVariant::fromValue(ui->splitter_2->sizes()));
  settings->setValue("settings/privacy", isPrivate);
  settings->setValue("settings/sonore", isSonore);
  settings->setValue("settings/language", lang);
  settings->setValue("settings/style", style);
}

/**
 * Load OpenJournal style stored at a given path.
 */
bool MainWindow::loadStyle(const QString path) {
  QFile stylesheet(path);
  if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {  // Read the theme file
    qApp->setStyleSheet(stylesheet.readAll());
    stylesheet.close();
    return true;
  }
  return false;
}

/**
 * Change OpenJournal language.
 */
void MainWindow::loadLanguages() {
  lang = settings->value("settings/language", "en").toString();
  QString appDir = QCoreApplication::applicationDirPath();
  QStringList translationLocation = {appDir, appDir + "/../Resources/", appDir + "/../share/openjournal/"};  // Windows, Macs bundle, Linux
  for (auto const &a : translationLocation) {
    QStringList availableLang = QDir(a).entryList({"*.qm"});
    if (!availableLang.isEmpty()) {
      translator.load("openjournal_" + lang, a);
      QLocale::setDefault(QLocale(lang));
      this->setLocale(QLocale(lang));
      qApp->installTranslator(&translator);
      for (const auto &i : availableLang) {  // Append one action by language
        QAction *langAction = new QAction(i.mid(12, 2), this);
        langAction->setCheckable(true);
        if (lang == i.mid(12, 2)) {
          langAction->setChecked(true);
        }
        connect(langAction, &QAction::triggered, [this, i]() {
          lang = i.mid(12, 2);
          reboot();
        });
        ui->menuLanguage->addAction(langAction);
      }
    }
  }
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About OpenJournal"), QString("<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>").arg(tr("OpenJournal"), QApplication::applicationVersion(), tr("A simple note taking journal, planner, reminder and Markdown editor."), tr("Copyright &copy; 2019-%1 Benjamin Gallois").arg("2021"), tr("Released under the <a href=%1>GPL 2</a> license").arg("\"https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html\"")));
}

//////////////// MainWindow events and tray /////////////////////////////////

/**
 * Event triggered when SystemTrayIcon is clicked.
 */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger: {
      break;
    }
    case QSystemTrayIcon::DoubleClick: {
      refreshTimer->start();
      ui->calendar->setSelectedDate(QDate::currentDate());
      refresh();
      this->setVisible(true);
      break;
    }
    default:;
  }
}

/**
 * Event triggered when OpenJournal closing is requested.
 * OpenJournal will close the MainWindow and stay in the tray, not updating any data except alarms.
 */
void MainWindow::closeEvent(QCloseEvent *event) {
  trayIcon->show();
  hide();
  if (isHelp) {
    trayIcon->showMessage(tr("Hey!"), tr("I'm there"), QIcon(), 1500);
    isHelp = false;
  }
  refreshTimer->stop();
  saveSettings();
  event->ignore();
}

/**
 * Blur the MainWindow when pointer exit window if privacy option is activated.
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (isPrivate) {
    if (event->type() == QEvent::Enter) {
      this->setGraphicsEffect(nullptr);
    }
    else if (event->type() == QEvent::HoverLeave) {
      QGraphicsBlurEffect *effect = new QGraphicsBlurEffect(this);
      effect->setBlurRadius(40);
      ui->preview->setGraphicsEffect(effect);
      this->setGraphicsEffect(effect);
    }
  }
  return false;
}

/**
 * Translate a part of the ui when required.
 * Necessary to reboot OpenJournal to translate the rest of the ui.
 */
void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    ui->retranslateUi(this);
  }
}

/**
 * Trigger OpenJournal reboot.
 */
void MainWindow::reboot() {
  saveSettings();
  qApp->quit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}
