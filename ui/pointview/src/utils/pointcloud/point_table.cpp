/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "point_table.h"

#include <QMessageBox>
#include <fstream>
#include <iostream>

#include "ui_point_table.h"
#include "utils.h"

namespace autox {
namespace pointview {

PointTable::PointTable(QWidget* parent, QStringList head, QString title)
    : QMainWindow(parent),
      ui(new Ui::PointTable),
      number_label_(new QLabel(this)),
      custom_function_layout_(new QHBoxLayout) {
  ui->setupUi(this);
  //  this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
  setHead(head);
  if (!title.isEmpty()) {
    this->setWindowTitle(title);
  }
  number_label_->setText("Point Number: ");
  ui->statusBar->addWidget(number_label_.get());
  // custom function layout
  ui->CustomFunctionLayout->insertLayout(0, custom_function_layout_.get());
  last_save_dirpath_ = QDir::homePath();
}

PointTable::~PointTable() { delete ui; }

void PointTable::addPoint(const std::vector<double>& point) {
  int rowCount = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(rowCount);
  int columnCount =
      std::min((int)ui->tableWidget->columnCount(), (int)point.size());
  for (int i = 0; i < columnCount; i++) {
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, point[i]);
    ui->tableWidget->setItem(rowCount, i, item);
  }
}

void PointTable::addPoint(const std::vector<QString>& point) {
  int rowCount = ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(rowCount);
  int columnCount =
      std::min((int)ui->tableWidget->columnCount(), (int)point.size());
  for (int i = 0; i < columnCount; i++) {
    QTableWidgetItem* item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, point[i].toDouble());
    ui->tableWidget->setItem(rowCount, i, item);
  }
}

void PointTable::on_SaveButton_clicked() {
  QString fileName = GetSaveFileName("Save point information to file",
                                     last_save_dirpath_ + "/Untitled.csv",
                                     "CSV Files(*.csv);;All Files(*.*)");
  if (fileName.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  last_save_dirpath_ = QFileInfo(fileName).dir().absolutePath();
  std::ofstream out_file(fileName.toStdString());
  if (!out_file) {
    return;
  }
  int rowCount = ui->tableWidget->rowCount();
  int columnCount = ui->tableWidget->columnCount();
  for (int i = 0; i < columnCount; i++) {
    auto head = ui->tableWidget->horizontalHeaderItem(i);
    out_file << head->text().toStdString();
    if (i != columnCount - 1) {
      out_file << ",";
    }
  }
  out_file << std::endl;
  for (int i = 0; i < rowCount; i++) {
    for (int j = 0; j < columnCount; j++) {
      auto item = ui->tableWidget->item(i, j);
      out_file << item->text().toStdString();
      if (j != columnCount - 1) {
        out_file << ",";
      }
    }
    out_file << std::endl;
  }
  out_file.close();
  QMessageBox msgBox;
  msgBox.setStyleSheet("QLabel{min-width: 200px;}");
  msgBox.setText("Success.");
  msgBox.setInformativeText("Save point information in " + fileName +
                            " successfully.");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void PointTable::cleanTable() {
  ui->tableWidget->clearContents();
  ui->tableWidget->setRowCount(0);
  refresh();
  ui->tableWidget->setSortingEnabled(false);
}

void PointTable::addMultiPoint(
    const std::vector<std::vector<QString>>& points) {}

void PointTable::refresh() {
  number_label_->setText("Point Number: " +
                         QString::number(ui->tableWidget->rowCount()));
  ui->tableWidget->setSortingEnabled(true);
}

void PointTable::setHead(QStringList head) {
  if (head.size() > 0) {
    ui->tableWidget->setColumnCount(head.size());
    ui->tableWidget->setHorizontalHeaderLabels(head);
  }
}

}  // namespace pointview
}  // namespace autox
