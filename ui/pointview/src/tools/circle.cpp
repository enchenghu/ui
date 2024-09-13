/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 3/15/24.
//

#include "circle.h"

Circle::Circle(std::shared_ptr<autox::pointview::DisplayContext> context)
    : autox::pointview::ToolBase(context, "Circle"),
      circle1m_checkbox_(new QCheckBox),
      circle10m_checkbox_(new QCheckBox),
      circle100m_checkbox_(new QCheckBox),
      height_spinbox_(new QDoubleSpinBox),
      range_spinbox_(new QSpinBox),
      one_start_spinbox_(new QSpinBox) {
  // show circle
  connect(circle1m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  connect(circle10m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  connect(circle100m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  addProperty("1m", circle1m_checkbox_);
  addProperty("10m", circle10m_checkbox_);
  addProperty("100m", circle100m_checkbox_);
  // range
  range_spinbox_->setSingleStep(100);
  range_spinbox_->setRange(100, 1000);
  range_spinbox_->setSuffix("m");
  range_spinbox_->setValue(circle_size_);
  connect(range_spinbox_.get(), SIGNAL(valueChanged(int)), this,
          SLOT(on_SizeSpinBoxChanged(int)));
  addProperty("range", range_spinbox_);
  // 1m start point
  one_start_spinbox_->setSingleStep(one_m_start_);
  one_start_spinbox_->setRange(1, 1000);
  one_start_spinbox_->setSuffix("m");
  one_start_spinbox_->setValue(1);
  connect(one_start_spinbox_.get(), SIGNAL(valueChanged(int)), this,
          SLOT(on_OneStartSpinBoxChanged(int)));
  addProperty("1m star point", one_start_spinbox_);
  // circle height
  height_spinbox_->setSingleStep(0.1);
  height_spinbox_->setRange(-5.0, 5.0);
  height_spinbox_->setSuffix("m");
  height_spinbox_->setValue(0);
  connect(height_spinbox_.get(), SIGNAL(valueChanged(double)), this,
          SLOT(on_HeightSpinBoxChanged(double)));
  addProperty("height", height_spinbox_);
}

Circle::~Circle() {}

void Circle::addCircleAxisText(const std::string& id, int axis, int value) {
  auto text = std::to_string(value) + "m";
  // color and size
  double r, g, b, size;
  if (value % 100 == 0) {
    // 100m
    size = 0.3;
    r = g = 1.0;
    b = 0;
  } else if (value % 10 == 0) {
    // 10m
    size = 0.2;
    r = g = b = 0.8;
  } else {
    // 1m
    size = 0.15;
    r = g = b = 0.6;
  }
  // pose
  pcl::PointXYZ position(0, 0, circle_height_);
  double orientation[3] = {0, 0, 0};
  if (axis == 0) {
    // x
    position.x = value + 0.1;
    orientation[2] = -90;
  } else if (axis == 1) {
    // y
    position.y = value + 0.1;
    orientation[0] = 0;
  }
  viewer_->addText3D(text, position, orientation, size, r, g, b, id);
}

void Circle::addCircle(int cell_count, bool show1m, bool show10m,
                       bool show100m) {
  int cnt = 0;

  for (int i = 1; i < cell_count; i++) {
    double r, g, b;
    r = g = b = 0;
    if (i % 100 == 0 && show100m) {
      // 100m
      r = g = 0.8;
      b = 0;
    } else if (i % 10 == 0 && show10m) {
      // 10m
      r = g = b = 0.5;
    } else {
      if ((!show1m) || i < one_m_start_ || i > one_m_start_ + 50) {
        continue;
      }
      // 1m
      g = b = 0.3;
    }
    int point_numer = 50;
    float radius = i;
    std::vector<pcl::PointXYZ> circle_points;
    double alpha = 2 * M_PI / (point_numer - 1);
    for (int j = 0; j < point_numer; j++) {
      pcl::PointXYZ point;
      point.x = radius * cos(j * alpha);
      point.y = radius * sin(j * alpha);
      point.z = circle_height_;
      circle_points.push_back(point);
    }
    for (int j = 0; j < point_numer - 1; j++) {
      viewer_->addLine(circle_points[j], circle_points[j + 1], r, g, b,
                       "circle" + std::to_string(cnt));
      cnt++;
    }
    viewer_->addLine(circle_points[point_numer - 1], circle_points[0], r, g, b,
                     "circle" + std::to_string(cnt));
    cnt++;
    addCircleAxisText("circle" + std::to_string(cnt), 0, i);
    cnt++;
    addCircleAxisText("circle" + std::to_string(cnt), 1, i);
    cnt++;
  }
  circle_shape_num_ = cnt;
}

void Circle::removeCircle() {
  for (int i = 0; i < circle_shape_num_; i++) {
    viewer_->removeShape("circle" + std::to_string(i));
  }
  circle_shape_num_ = 0;
}

void Circle::on_ShowCheckBoxStateChanged() {
  removeCircle();
  addCircle(circle_size_, circle1m_checkbox_->isChecked(),
            circle10m_checkbox_->isChecked(),
            circle100m_checkbox_->isChecked());
  emit refreshView();
}

void Circle::on_SizeSpinBoxChanged(int value) {
  if (circle_size_ == value) {
    return;
  }
  circle_size_ = value;
  on_ShowCheckBoxStateChanged();
}

void Circle::on_HeightSpinBoxChanged(double value) {
  if (circle_height_ == value) {
    return;
  }
  circle_height_ = value;
  on_ShowCheckBoxStateChanged();
}

void Circle::on_OneStartSpinBoxChanged(int value) {
  if (one_m_start_ == value) {
    return;
  }
  one_m_start_ = value;
  on_ShowCheckBoxStateChanged();
}
