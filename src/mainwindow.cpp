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
  settings = new QSettings();
  this->resize(settings->value("mainwindow/size", QSize(400, 400)).toSize());
  this->move(settings->value("mainwindow/pos", QPoint(200, 200)).toPoint());

  // Tray integration
  trayIcon = new QSystemTrayIcon(QIcon(":/openjournal.svg"), this);
  QMenu *trayMenu = new QMenu;
  QAction *restore = new QAction(tr("Restore"));
  connect(restore, &QAction::triggered, [this]() {
    this->showNormal();
    ui->calendar->setSelectedDate(QDate::currentDate());
    refreshTimer->start();
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
    clearJournalPage();
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
  connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::loadJournalPage);

  // Journal page
  page = new JournalPage();
  connect(ui->entry, &QPlainTextEdit::textChanged, page, [this]() {
    QString entry = ui->entry->toPlainText();
    page->setEntry(entry);
  });
  connect(page, &JournalPage::getDate, ui->date, &QLabel::setText);
  connect(page, &JournalPage::getEntry, ui->entry, &QPlainTextEdit::setPlainText);

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
  QAction *actionAddAlarm = new QAction(QIcon(":/clocks.png"), "Add Alarm", this);
  connect(actionAddAlarm, &QAction::triggered, [this]() {
    ui->entry->moveCursor(QTextCursor::End);
    AddAlarm *alarm = new AddAlarm();
    connect(alarm, &AddAlarm::alarm, ui->entry, &QMarkdownTextEdit::appendPlainText);
    alarm->exec();
    delete alarm;
  });
  ui->toolBar->addAction(actionAddAlarm);

  QAction *actionToDoTemplate = new QAction(QIcon(":/todo.png"), "To do list template", this);
  connect(actionToDoTemplate, &QAction::triggered, this, &MainWindow::insertToDoTemplate);
  ui->toolBar->addAction(actionToDoTemplate);

  QAction *actionTableTemplate = new QAction(QIcon(":/table.png"), "Table template", this);
  connect(actionTableTemplate, &QAction::triggered, this, &MainWindow::insertTableTemplate);
  ui->toolBar->addAction(actionTableTemplate);

  QAction *actionLock = new QAction(QIcon(":/lock.png"), "Lock journal", this);
  actionLock->setCheckable(true);
  actionLock->setChecked(false);
  connect(actionLock, &QAction::triggered, ui->entry, &QMarkdownTextEdit::setDisabled);
  connect(actionLock, &QAction::triggered, [this](bool status) {
    if (status) {
      ui->statusBar->showMessage("The journal is locked");
    }
  });
  ui->toolBar->addAction(actionLock);
  addToolBar(Qt::LeftToolBarArea, ui->toolBar);

  // Planner exportation
  connect(ui->actionExport_planner, &QAction::triggered, page, &JournalPage::readFromDatabaseAll);
  connect(page, &JournalPage::getAll, [this](QString data) {
    doc.setText(data);
    emit(exportLoadingFinished());
  });
  connect(this, &MainWindow::exportLoadingFinished, this, &MainWindow::exportAll);

  // Automatic refresh every 10 seconds
  refreshTimer = new QTimer(this);
  connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refresh);
  refreshTimer->start(10000);
  refresh();

  // Automatic today focus every hour
  QTimer *todayTimer = new QTimer(this);
  connect(
      todayTimer, &QTimer::timeout, todayTimer, [this]() {
        ui->calendar->setSelectedDate(QDate::currentDate());
        loadJournalPage(ui->calendar->selectedDate());
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
  QAction *isSonoreAction = new QAction("Alarm Sound");
  isSonoreAction->setCheckable(true);
  isSonoreAction->setChecked(isSonore);
  connect(isSonoreAction, &QAction::triggered, [this](bool state) {
    isSonore = state;
  });
  ui->menuOptions->addAction(isSonoreAction);
  this->installEventFilter(this);
}

void MainWindow::loadJournalPage(const QDate date) {
  page->setReadOnly(true);  // Prevent erasing old entry
  ui->entry->clear();
  page->setReadOnly(false);
  page->setDate(date);
  page->readFromDatabase();
}

void MainWindow::newJournal() {
  clearJournalPage();
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
    loadJournalPage(QDate::currentDate());
  }
}

void MainWindow::openJournal() {
  clearJournalPage();
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
    loadJournalPage(QDate::currentDate());
    statusMessage->setText(plannerName + tr(" journal is opened"));
    ui->actionBackup->setEnabled(true);
    ui->entry->setEnabled(true);
  }
}

void MainWindow::openJournal(QString hostname, QString port, QString username, QString password, QString plannerFile) {
  clearJournalPage();
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
    loadJournalPage(QDate::currentDate());
    query.prepare("CREATE TABLE journalPage (date TEXT, entry TEXT)");
    query.exec();
  }
  else {
    statusMessage->setText(tr("No journal is opened"));
  }
}

void MainWindow::clearJournalPage() {
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
}

void MainWindow::backup() {
  QFile file(plannerName);
  QFile copy(plannerName + ".back");
  if (!copy.exists()) {
    file.copy(plannerName + ".back");
    ui->statusBar->showMessage("Journal was backed up " + plannerName + ".back");
  }
  else if (copy.exists()) {
    copy.remove();
    file.copy(plannerName + ".back");
    ui->statusBar->showMessage("Cannot back up journal");
  }
}

MainWindow::~MainWindow() {
  delete ui;
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
  loadJournalPage(ui->calendar->selectedDate());
  QTextCursor cursor = ui->entry->textCursor();
  cursor.setPosition(cursorPosition);
  ui->entry->setTextCursor(cursor);
  QTime time = QTime::currentTime();
  clock->display(time.toString("hh:mm"));
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About OpenJournal"), QString("<p align='center'><big><b>%1 %2</b></big><br/>%3<br/><small>%4<br/>%5</small></p>").arg(tr("OpenJournal"), QApplication::applicationVersion(), tr("A simple note taking journal, planner, reminder."), tr("Copyright &copy; 2019-%1 Benjamin Gallois").arg("2021"), tr("Released under the <a href=%1>GPL 3</a> license").arg("\"http://www.gnu.org/licenses/gpl.html\"")));
}

void MainWindow::insertToDoTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\n### To do list\n\n- [x] Task 1\n- [ ] Task 2\n- [ ] Task 3\n");
  }
}

void MainWindow::insertTableTemplate() {
  if (ui->entry->isEnabled()) {
    ui->entry->insertPlainText("\nA | B | C\n--- | --- | ---\nA1 | B1 | C1\nA2 | B2 | C2\nA3 | B3 | C3\n\n");
  }
}
