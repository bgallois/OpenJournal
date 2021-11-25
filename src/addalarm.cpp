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
