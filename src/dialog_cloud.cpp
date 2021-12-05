#include "dialog_cloud.h"
#include "ui_dialog_cloud.h"

DialogCloud::DialogCloud(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::DialogCloud) {
  ui->setupUi(this);
}

DialogCloud::~DialogCloud() {
  delete ui;
}

QPair<QString, QString> DialogCloud::getValues() {
  QPair<QString, QString> pair(ui->user->text(), ui->password->text());
  return pair;
}

void DialogCloud::setValue(QString user) {
  ui->user->setText(user);
}
