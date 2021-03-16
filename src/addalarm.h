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
