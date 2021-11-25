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

#ifndef ADDALARM_H
#define ADDALARM_H

#include <QDialog>
#include <QTime>

namespace Ui {
class AddAlarm;
}

class AddAlarm : public QDialog {
  Q_OBJECT

 public:
  explicit AddAlarm(QWidget *parent = nullptr);
  ~AddAlarm();

 private:
  Ui::AddAlarm *ui;

 public slots:
  void set();

 signals:
  void alarm(QString);
};

#endif  // ADDALARM_H
