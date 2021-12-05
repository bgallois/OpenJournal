#ifndef DIALOG_CLOUD_H
#define DIALOG_CLOUD_H

#include <QDialog>
#include <QPair>
#include <QString>

namespace Ui {
class DialogCloud;
}

class DialogCloud : public QDialog {
  Q_OBJECT

 public:
  explicit DialogCloud(QWidget *parent = nullptr);
  ~DialogCloud();

 public slots:
  QPair<QString, QString> getValues();
  void setValue(QString user);

 private:
  Ui::DialogCloud *ui;
};

#endif  // DIALOG_CLOUD_H
