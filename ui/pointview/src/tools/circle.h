/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 3/15/24.
//

#ifndef POINTVIEW_CIRCLE_H
#define POINTVIEW_CIRCLE_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include "utils/common/tool_base.h"
#include "utils/xqtwidget.h"

class Circle : public autox::pointview::ToolBase {
  Q_OBJECT
 public:
  explicit Circle(std::shared_ptr<autox::pointview::DisplayContext> context);
  ~Circle() override;
 private slots:
  void on_ShowCheckBoxStateChanged();
  void on_SizeSpinBoxChanged(int value);
  void on_HeightSpinBoxChanged(double value);
  void on_OneStartSpinBoxChanged(int value);

 private:
  void addCircleAxisText(const std::string& id, int axis, int value);
  void addCircle(int cell_count, bool show1m, bool show10m, bool show100m);
  void removeCircle();

 private:
  int circle_shape_num_{0};
  int circle_size_ = 100;
  double circle_height_ = 0.0;
  int one_m_start_{1};
  // item
  std::shared_ptr<QCheckBox> circle1m_checkbox_;
  std::shared_ptr<QCheckBox> circle10m_checkbox_;
  std::shared_ptr<QCheckBox> circle100m_checkbox_;
  std::shared_ptr<QSpinBox> range_spinbox_;
  std::shared_ptr<QSpinBox> one_start_spinbox_;
  std::shared_ptr<QDoubleSpinBox> height_spinbox_;
};

#endif  // POINTVIEW_CIRCLE_H
