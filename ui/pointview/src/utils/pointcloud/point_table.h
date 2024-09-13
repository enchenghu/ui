/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTTABLE_H
#define POINTTABLE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QSpacerItem>
#include <algorithm>
#include <memory>
#include <vector>

namespace Ui {
class PointTable;
}

namespace autox {
namespace pointview {

class PointTable : public QMainWindow {
  Q_OBJECT
 public:
  explicit PointTable(QWidget* parent = 0, QStringList head = {},
                      QString title = "");
  ~PointTable();
  void addPoint(const std::vector<double>& point);
  void addPoint(const std::vector<QString>& point);
  void refresh();
  void addMultiPoint(const std::vector<std::vector<QString>>& points);
  void cleanTable();
  void setHead(QStringList head);

 private slots:
  void on_SaveButton_clicked();

 protected:
  Ui::PointTable* ui;
  std::shared_ptr<QLabel> number_label_;
  std::shared_ptr<QHBoxLayout> custom_function_layout_;
  QString last_save_dirpath_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTTABLE_H
