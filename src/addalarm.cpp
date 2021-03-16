#include "addalarm.h"
#include "ui_addalarm.h"

AddAlarm::AddAlarm(QWidget *parent) : QDialog(parent),
                                      ui(new Ui::AddAlarm) {
  ui->setupUi(this);
  ui->time->setTime(QTime::currentTime());
  connect(ui->setButton, &QPushButton::clicked, this, &AddAlarm::set);
}

AddAlarm::~AddAlarm() {
  delete ui;
}

void AddAlarm::set() {
  QTime time = ui->time->time();
  QString message = ui->message->toPlainText();
  emit(alarm("setAlarm(" + time.toString("hh:mm") + "," + message + ");"));
  this->close();
}
