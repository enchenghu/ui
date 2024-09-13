/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 1/11/24.
//

#ifndef POINTVIEW_LOG_VIEWER_H
#define POINTVIEW_LOG_VIEWER_H

#include <QMainWindow>
#include <QPushButton>
#include <string>

namespace Ui {
class LogViewerUi;
}

namespace autox::pointview {

class LogViewer : public QMainWindow {
  Q_OBJECT
 public:
  LogViewer(QWidget* parent = nullptr);
  ~LogViewer() override;
  void open();
 public slots:
  void addText(const QString str);
  void addText(const std::string str);
 private slots:
  void on_clearButtonClicked();
  void on_saveButtonClicked();

 private:
  Ui::LogViewerUi* ui;
};

}  // namespace autox::pointview

#endif  // POINTVIEW_LOG_VIEWER_H
