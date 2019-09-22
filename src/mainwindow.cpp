#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  ui->setupUi(this);

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

  // Preview
  previewPage = new PreviewPage(this);
  ui->preview->setPage(previewPage);
  connect(ui->entry, &QPlainTextEdit::textChanged,[this]() { 
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

  // Connect calendar
  connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::loadJournalPage);
 
  // Initialize a default planner page
  // To investigate => no error on fedora, error with ubuntu
  page = new JournalPage(db, QDate::currentDate());
  reminders = new QStringList;

  // Reads settings
  QSettings settings("OpenJournal", "B&GInc");
  QString lastJournal = settings.value("mainwindow/lastJournal").toString();
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
  connect(refreshTimer, &QTimer::timeout, [this](){
    ui->calendar->setSelectedDate(QDate::currentDate());
    loadJournalPage(QDate::currentDate());
    backup();
  });

}

void MainWindow::loadJournalPage(const QDate date) {
  delete page;
  delete reminders;
  ui->entry->clear();
  page = new JournalPage(db, date);
  reminders = new QStringList();

  // Sends values from ui to page object
  connect(ui->entry, &QPlainTextEdit::textChanged, page, [this](){
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
  QSettings settings("OpenJournal", "B&GInc");
  settings.setValue("mainwindow/lastJournal", plannerName);
}

void MainWindow::backup() {
  QFile file(plannerName);
  QFile copy(plannerName + ".back");
  if(!copy.exists()){
    file.copy(plannerName + ".back");
  }
  else if(copy.exists()){
    copy.remove();
    file.copy(plannerName + ".back");
  }
}

MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
  switch (reason) {
    case QSystemTrayIcon::Trigger:
    {
      this->setVisible(true);
      break;
    }
    case QSystemTrayIcon::DoubleClick:
    {
      break;
    }
    default:
        ;
  }
}

void MainWindow::closeEvent(QCloseEvent *event){
  trayIcon->show();
  hide();
  event->ignore();
}

void MainWindow::reminder(QString text, QStringList *reminders) {
  if(QDate::currentDate() != ui->calendar->selectedDate()){return;}
  int end = 0;
  while (end >= 0) {
    int start = text.indexOf("setReminder(", end);
    if( start != -1){
      end = text.indexOf(");", start);
      if(end != -1){
        QStringRef command = text.midRef(start + 12, end - start - 12); 
        QVector<QStringRef> commands = command.split(',', QString::SkipEmptyParts);
        if(commands.length() == 2 && !reminders->contains(commands[1].toString())){
          QVector<QStringRef> timeSet = commands[0].split('h');
          int time = - QTime(timeSet[0].toDouble(), timeSet[1].toDouble()).msecsTo(QTime::currentTime());
          if(time > 0){
            qInfo() << commands;
            QTimer *notification = new QTimer(page);
            notification->setSingleShot(true);
            notification->setInterval(time);
            QString message = commands[1].toString();
            connect(notification, &QTimer::timeout, [this, message](){
              trayIcon->showMessage("Notification", message, QIcon(), 214483648);
            });
            notification->start();
          }
        }
      }
    }
    else{
      break;
    }
  }
}

