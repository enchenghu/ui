//
// Created by tongdayang on 11/29/22.
//

#include "coordinate_axis.h"

CoordinateAxis::CoordinateAxis(
    std::shared_ptr<autox::pointview::DisplayContext> context)
    : autox::pointview::ToolBase(context, "CoordinateAxis"),
      show_checkbox_(new QCheckBox),
      x_spinbox_(new QDoubleSpinBox),
      y_spinbox_(new QDoubleSpinBox),
      z_spinbox_(new QDoubleSpinBox),
      scale_spinbox_(new QDoubleSpinBox) {
  show_checkbox_->setObjectName("ShowCheckBox");
  connect(show_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged(int)));
  addProperty("Show", show_checkbox_);

  x_spinbox_->setSingleStep(0.1);
  y_spinbox_->setSingleStep(0.1);
  z_spinbox_->setSingleStep(0.1);
  scale_spinbox_->setSingleStep(0.1);
  scale_spinbox_->setRange(0.5, 5.0);
  addProperty("x", x_spinbox_);
  addProperty("y", y_spinbox_);
  addProperty("z", z_spinbox_);
  addProperty("scale", scale_spinbox_);
}
CoordinateAxis::~CoordinateAxis() {}
void CoordinateAxis::on_ShowCheckBoxStateChanged(int state) {
  if (state > 0) {
    double x, y, z, scale;
    x = x_spinbox_->value();
    y = y_spinbox_->value();
    z = z_spinbox_->value();
    scale = scale_spinbox_->value();
    viewer_->addCoordinateSystem(scale, x, y, z);
  } else {
    viewer_->removeCoordinateSystem();
  }
  emit refreshView();
}
