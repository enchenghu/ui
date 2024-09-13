/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 1/11/24.
//

#include "log_viewer.h"

#include <QMessageBox>
#include <fstream>
#include <iostream>

#include "ui_log_viewer_ui.h"
#include "utils.h"

namespace autox::pointview {
LogViewer::LogViewer(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::LogViewerUi) {
  ui->setupUi(this);
  ui->BrowserWindow->setReadOnly(true);
  connect(ui->ClearButton, SIGNAL(clicked(bool)), this,
          SLOT(on_clearButtonClicked()));
  connect(ui->SaveButton, SIGNAL(clicked(bool)), this,
          SLOT(on_saveButtonClicked()));
}

LogViewer::~LogViewer() { delete ui; }

void LogViewer::on_clearButtonClicked() { ui->BrowserWindow->clear(); }

void LogViewer::on_saveButtonClicked() {
  QString fileName =
      GetSaveFileName("Save log to file", QDir::homePath() + "/Untitled.txt",
                      "TXT Files(*.txt);;All Files(*.*)");
  if (fileName.isNull()) {
    Debug("Do not select a target file.");
    return;
  }

  QString str = ui->BrowserWindow->toPlainText();
  QMessageBox msgBox;
  msgBox.setStyleSheet("QLabel{min-width: 200px;}");
  std::ofstream out(fileName.toStdString());
  out << str.toStdString();
  out.close();
  msgBox.setText("Success.");
  msgBox.setInformativeText("Save log in " + fileName + " successfully.");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void LogViewer::open() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void LogViewer::addText(const QString str) {
  ui->BrowserWindow->appendPlainText(str);
}

void LogViewer::addText(const std::string str) {
  addText(QString::fromStdString(str));
}
}  // namespace autox::pointview