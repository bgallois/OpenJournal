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

  // Translation
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
  connect(ui->actionBackup, &QAction::triggered, this, &MainWindow::backup);
  connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);
  connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
  connect(ui->actionSource, &QAction::triggered, [this]() {
    QDesktopServices::openUrl(QUrl("https://github.com/bgallois/OpenJournal", QUrl::TolerantMode));
  });

  // Connect calendar
  connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::loadJournal);

  // Journal page
  page = new Journal();
  connect(ui->entry, &QPlainTextEdit::textChanged, page, [this]() {
    QString entry = ui->entry->toPlainText();
    page->setEntry(entry);
  });
  connect(page, &Journal::getDate, ui->date, &QLabel::setText);
  connect(page, &Journal::getEntry, ui->entry, &QPlainTextEdit::setPlainText);

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

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_D);
  QAction *actionToDoTemplate = new QAction(QIcon(":/todo.png"), tr("To do list template ") + keySequence.toString(), this);
  actionToDoTemplate->setShortcut(keySequence);
  connect(actionToDoTemplate, &QAction::triggered, this, &MainWindow::insertToDoTemplate);
  ui->toolBar->addAction(actionToDoTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_L);
  QAction *actionListNumberedTemplate = new QAction(QIcon(":/listNumbered.png"), tr("Numbered list ") + keySequence.toString(), this);
  actionListNumberedTemplate->setShortcut(keySequence);
  connect(actionListNumberedTemplate, &QAction::triggered, this, &MainWindow::insertListNumberedTemplate);
  ui->toolBar->addAction(actionListNumberedTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L);
  QAction *actionListTemplate = new QAction(QIcon(":/list.png"), tr("Unordered list ") + keySequence.toString(), this);
  actionListTemplate->setShortcut(keySequence);
  connect(actionListTemplate, &QAction::triggered, this, &MainWindow::insertListTemplate);
  ui->toolBar->addAction(actionListTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_T);
  QAction *actionTableTemplate = new QAction(QIcon(":/table.png"), tr("Table template ") + keySequence.toString(), this);
  actionTableTemplate->setShortcut(keySequence);
  connect(actionTableTemplate, &QAction::triggered, this, &MainWindow::insertTableTemplate);
  ui->toolBar->addAction(actionTableTemplate);

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_U);
  QAction *actionLinkTemplate = new QAction(QIcon(":/link.png"), tr("URL template ") + keySequence.toString(), this);
  actionLinkTemplate->setShortcut(keySequence);
  connect(actionLinkTemplate, &QAction::triggered, this, &MainWindow::insertLinkTemplate);
  ui->toolBar->addAction(actionLinkTemplate);
  ui->toolBar->addSeparator();

  keySequence = QKeySequence(Qt::CTRL + Qt::Key_Escape);
  QAction *actionLock = new QAction(QIcon(":/lock.png"), tr("Lock journal ") + keySequence.toString(), this);
  actionLock->setShortcut(keySequence);
  actionLock->setCheckable(true);
  actionLock->setChecked(false);
  connect(actionLock, &QAction::triggered, ui->entry, &QMarkdownTextEdit::setDisabled);
  connect(actionLock, &QAction::triggered, [this](bool status) {
    if (status) {
      ui->statusBar->showMessage(tr("The journal is locked"));
    }
    else {
      ui->statusBar->showMessage(tr("The journal is unlocked"), 2000);
    }
  });
  ui->toolBar->addAction(actionLock);
  addToolBar(Qt::LeftToolBarArea, ui->toolBar);

  // Planner exportation
  connect(ui->actionExport_planner, &QAction::triggered, page, &Journal::readFromDatabaseAll);
  connect(page, &Journal::getAll, [this](QString data) {
    doc.setText(data);
    emit(exportLoadingFinished());
  });
  connect(this, &MainWindow::exportLoadingFinished, this, &MainWindow::exportAll);

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

void MainWindow::loadJournal(const QDate date) {
  if (page->isActive()) {
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

void MainWindow::newJournal(QString fileName) {
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
    QSqlQuery query("CREATE TABLE journalPage (date TEXT, entry TEXT)");
    ui->calendar->setSelectedDate(QDate::currentDate());
    page->setDatabase(db);
    loadJournal(QDate::currentDate());
  }
}

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

void MainWindow::openJournal(QString plannerFile) {
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

void MainWindow::openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile) {
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
    QSqlQuery query(db);
    query.prepare("CREATE TABLE journalPage (date TEXT, entry TEXT)");
    query.exec();
    plannerName = plannerFile;
    ui->actionBackup->setEnabled(false);
    statusMessage->setText(plannerName + tr(" journal is opened"));
    ui->entry->setEnabled(true);
    ui->calendar->setSelectedDate(QDate::currentDate());
    page->setDatabase(db);
    loadJournal(QDate::currentDate());
    query.prepare("CREATE TABLE journalPage (date TEXT, entry TEXT)");
    query.exec();
  }
  else {
    statusMessage->setText(tr("No journal is opened"));
  }
}

void MainWindow::clearJournal() {
  page->writeToDatabase();
  db.close();
  ui->date->clear();
  ui->entry->clear();
}

void MainWindow::saveSettings() {
  settings->setValue("mainwindow/lastJournal", plannerName);
  settings->setValue("mainwindow/size", this->size());
  settings->setValue("mainwindow/pos", this->pos());
  settings->setValue("settings/privacy", isPrivate);
  settings->setValue("settings/sonore", isSonore);
  settings->setValue("settings/language", lang);
}

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

MainWindow::~MainWindow() {
  delete ui;
  delete page;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger: {
      break;
    }
    case QSystemTrayIcon::DoubleClick: {
      refreshTimer->start();
      ui->calendar->setSelectedDate(QDate::currentDate());
      this->setVisible(true);
      break;
    }
    default:;
  }
}

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

void MainWindow::exportAll() {
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Select file"), QDir::homePath(), tr("Pdf Files (*.pdf)"));
  if (!fileName.isEmpty()) {
    ui->preview->page()->printToPdf(fileName);
  }
}

void MainWindow::refresh() {
  int cursorPosition = ui->entry->textCursor().position();
  int cursorAnchor = ui->entry->textCursor().anchor();
  loadJournal(ui->calendar->selectedDate());
  QTextCursor cursor = ui->entry->textCursor();
  cursor.setPosition(cursorAnchor);
  cursor.setPosition(cursorPosition, QTextCursor::KeepAnchor);
  ui->entry->setTextCursor(cursor);
  QTime time = QTime::currentTime();
  clock->display(time.toString("hh:mm"));
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About OpenJournal"), QString("<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>").arg(tr("OpenJournal"), QApplication::applicationVersion(), tr("A simple note taking journal, planner, reminder."), tr("Copyright &copy; 2019-%1 Benjamin Gallois").arg("2021"), tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\"")));
}

void MainWindow::insertToDoTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\n\n### To do list\n\n- [x] Task 1\n- [ ] Task 2\n- [ ] Task 3\n\n");
  }
}

void MainWindow::insertTableTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\n\nA | B | C\n--- | --- | ---\nA1 | B1 | C1\nA2 | B2 | C2\nA3 | B3 | C3\n\n");
  }
}

void MainWindow::insertLinkTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText(" [Uniform Resource Locator](https://github.com/bgallois/OpenJournal)");
  }
}

void MainWindow::insertListNumberedTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\n\n1. Item 1\n2. Item 2\n3. Item 3\n\n");
  }
}

void MainWindow::insertListTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\n\n* Item 1\n* Item 2\n* Item 3\n\n");
  }
}

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    ui->retranslateUi(this);
  }
}

void MainWindow::reboot() {
  saveSettings();
  qApp->quit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}
