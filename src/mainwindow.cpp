#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  ui->setupUi(this);

  settings = new QSettings("OpenJournal", "B&GInc");
  this->resize(settings->value("size", QSize(400, 400)).toSize());
  this->move(settings->value("pos", QPoint(200, 200)).toPoint());

  trayIcon = new QSystemTrayIcon(QIcon(":/openjournal.svg"));
  QMenu *trayMenu = new QMenu;
  QAction *restore = new QAction(tr("Restore"));
  connect(restore, &QAction::triggered, this, &MainWindow::showNormal);
  trayMenu->addAction(restore);
  QAction *close = new QAction(tr("Close"));
  connect(close, &QAction::triggered, qApp, &QCoreApplication::quit);
  trayMenu->addAction(close);
  connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  alarmSound = new QSoundEffect(this);
  alarmSound->setSource(QUrl("qrc:/notification_0.wav"));
  alarmSound->setLoopCount(0);
  alarmSound->setVolume(0.70);
  connect(trayIcon, &QSystemTrayIcon::messageClicked, alarmSound, &QSoundEffect::stop);
  connect(trayIcon, &QSystemTrayIcon::activated, alarmSound, &QSoundEffect::stop);

  // Preview
  previewPage = new PreviewPage(this);
  ui->preview->setPage(previewPage);
  connect(ui->entry, &QPlainTextEdit::textChanged, [this]() {
    this->doc.setText(ui->entry->toPlainText());
    this->reminder(ui->entry->toPlainText(), reminders);
  });
  QWebChannel *channel = new QWebChannel(this);
  channel->registerObject(QStringLiteral("content"), &doc);
  previewPage->setWebChannel(channel);
  ui->preview->setUrl(QUrl("qrc:/index.html"));

  // Connect menu bar
  connect(ui->actionNew_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::newJournal));
  connect(ui->actionOpen_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::openJournal));
  connect(ui->actionBackup, &QAction::triggered, this, &MainWindow::backup);
  connect(ui->actionAddAlarm, &QAction::triggered, [this]() {
    AddAlarm *alarm = new AddAlarm();
    connect(alarm, &AddAlarm::alarm, ui->entry, &QMarkdownTextEdit::appendPlainText);
    alarm->exec();
    delete alarm;
  });

  // Connect calendar
  connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::loadJournalPage);

  // Initialize a default planner page
  // To investigate => no error on fedora, error with ubuntu
  page = new JournalPage(db, QDate::currentDate());
  reminders = new QStringList;

  // Reads settings
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

  // Automatic refresh
  refreshTimer = new QTimer(this);
  refreshTimer->start(3600000);
  connect(refreshTimer, &QTimer::timeout, [this]() {
    ui->calendar->setSelectedDate(QDate::currentDate());
    loadJournalPage(QDate::currentDate());
    backup();
  });

  // Privacy
  isPrivate = settings->value("settings/privacy").toBool();
  QAction *isPrivateAction = new QAction("Private mode");
  isPrivateAction->setCheckable(true);
  isPrivateAction->setChecked(isPrivate);
  connect(isPrivateAction, &QAction::triggered, [this](bool state) {
    isPrivate = state;
  });
  ui->menuOptions->addAction(isPrivateAction);
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
  delete page;
  delete reminders;
  ui->entry->clear();
  page = new JournalPage(db, date);
  reminders = new QStringList();

  // Sends values from ui to page object
  connect(ui->entry, &QPlainTextEdit::textChanged, page, [this]() {
    QString entry = ui->entry->toPlainText();
    page->setEntry(entry);
  });

  // Gets values from page object to the ui
  connect(page, &JournalPage::getDate, ui->date, &QLabel::setText);
  connect(page, &JournalPage::getEntry, ui->entry, &QPlainTextEdit::setPlainText);
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
  if (!db.open()) {
    plannerName = fileName;
    ui->statusBar->showMessage(plannerName + " journal failed to open");
    return;
  }
  else {
    ui->statusBar->showMessage(plannerName + " journal is opened");
    plannerName = plannerName;
  }
  QSqlQuery query("CREATE TABLE journalPage (date TEXT, entry TEXT)");
  query.isActive();

  ui->calendar->setSelectedDate(QDate::currentDate());
  loadJournalPage(QDate::currentDate());
}

void MainWindow::openJournal() {
  clearJournalPage();
  plannerName = QFileDialog::getOpenFileName(this, tr("Open journal"), "", tr("Journal (*.jnl)"));
  openJournal(plannerName);
}

void MainWindow::openJournal(QString plannerFile) {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(plannerFile);
  if (!db.open()) {
    ui->statusBar->showMessage(plannerFile + " journal failed to open");
    return;
  }
  else {
    plannerName = plannerFile;
    ui->statusBar->showMessage(plannerName + " journal is opened");
  }

  ui->calendar->setSelectedDate(QDate::currentDate());
  loadJournalPage(QDate::currentDate());
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
  }
  else if (copy.exists()) {
    copy.remove();
    file.copy(plannerName + ".back");
  }
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::Trigger: {
      this->setVisible(true);
      break;
    }
    case QSystemTrayIcon::DoubleClick: {
      break;
    }
    default:;
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  trayIcon->show();
  hide();
  event->ignore();
  saveSettings();
}

void MainWindow::reminder(QString text, QStringList *reminders) {
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
        QVector<QStringRef> commands = command.split(',', QString::SkipEmptyParts);
        if (commands.length() == 2 && !reminders->contains(commands[1].toString())) {
          QVector<QStringRef> timeSet = commands[0].split(':');
          int time = -QTime(timeSet[0].toDouble(), timeSet[1].toDouble()).msecsTo(QTime::currentTime());
          if (time > 0) {
            QTimer *notification = new QTimer(page);
            notification->setSingleShot(true);
            notification->setInterval(time);
            QString message = commands[1].toString();
            connect(notification, &QTimer::timeout, [this, message]() {
              trayIcon->showMessage("Notification", message, QIcon(), 214483648);
              if (isSonore) {
                alarmSound->play();
              }
            });
            notification->start();
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
