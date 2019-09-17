#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // Preview
  previewPage = new PreviewPage(this);
  ui->preview->setPage(previewPage);
  connect(ui->entry, &QPlainTextEdit::textChanged,[this]() { 
    doc.setText(ui->entry->toPlainText());
    });
  QWebChannel *channel = new QWebChannel(this);
  channel->registerObject(QStringLiteral("content"), &doc);
  previewPage->setWebChannel(channel);
  ui->preview->setUrl(QUrl("qrc:/index.html"));


  // Connect menu bar
  connect(ui->actionNew_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::newJournal));
  connect(ui->actionOpen_planner, &QAction::triggered, this, QOverload<>::of(&MainWindow::openJournal));

  // Connect calendar
  connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::loadJournalPage);
 
  // Initialize a default planner page
  // To investigate => no error on fedora, error with ubuntu
  page = new JournalPage(db, QDate::currentDate());

  // Reads settings
  QSettings settings("OpenJournal", "B&GInc");
  QString lastJournal = settings.value("mainwindow/lastJournal").toString();
  if (!lastJournal.isEmpty()) {
    openJournal(lastJournal);
  }
  else if (QDir(QDir::homePath() + "/OpenJournal.jnl").exists()) {
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
  });

}

void MainWindow::loadJournalPage(const QDate date) {
  delete page;
  ui->entry->clear();
  page = new JournalPage(db, date);

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


MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}
