//
// Created by tongdayang on 11/29/22.
//

#ifndef POINT_VIEW_COORDINATE_AXIS_H
#define POINT_VIEW_COORDINATE_AXIS_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "utils/common/tool_base.h"
#include "utils/xqtwidget.h"

class CoordinateAxis : public autox::pointview::ToolBase {
  Q_OBJECT
 public:
  explicit CoordinateAxis(std::shared_ptr<autox::pointview::DisplayContext> context);
  ~CoordinateAxis() override;
 private slots:
  void on_ShowCheckBoxStateChanged(int state);

 private:
  std::shared_ptr<QCheckBox> show_checkbox_;
  std::shared_ptr<QDoubleSpinBox> x_spinbox_;
  std::shared_ptr<QDoubleSpinBox> y_spinbox_;
  std::shared_ptr<QDoubleSpinBox> z_spinbox_;
  std::shared_ptr<QDoubleSpinBox> scale_spinbox_;
};

#endif  // POINT_VIEW_COORDINATE_AXIS_H
