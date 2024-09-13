/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 5/4/23.
//

#include "point_distance_measure.h"
PointDistanceMeasure::PointDistanceMeasure(
    std::shared_ptr<autox::pointview::DisplayContext> context)
    : autox::pointview::ToolBase(context, "PointDistanceMeasure"),
      sphere_checkbox_(new QCheckBox) {
  addProperty("Bold Point: ", sphere_checkbox_);
  viewer_->registerPointPickingCallback(
      &PointDistanceMeasure::PointPickCallback, *this);
}

PointDistanceMeasure::~PointDistanceMeasure() {}

void PointDistanceMeasure::PointPickCallback(
    const pcl::visualization::PointPickingEvent& event, void* args) {
  float x, y, z;
  event.getPoint(x, y, z);
  if (is_second_point_) {
    is_second_point_ = false;
    float dis_x = x - last_x_;
    float dis_y = y - last_y_;
    float dis_z = z - last_z_;
    float dis = dis_x * dis_x + dis_y * dis_y + dis_z * dis_z;
    dis = std::sqrt(dis);
    std::cout << "PointDistanceMeasure, dx:" << dis_x << ", dy:" << dis_y
              << ", dz:" << dis_z << ", d:" << dis << " m" << std::endl;
    if (sphere_checkbox_->isChecked()) {
      viewer_->addSphere(pcl::PointXYZ(x, y, z), 0.05, 1.0, 0.0, 0.0, "Point2");
    }

    viewer_->addLine(pcl::PointXYZ(x, y, z),
                     pcl::PointXYZ(last_x_, last_y_, last_z_), 1.0, 0.0, 0.0,
                     "PointLine");
    viewer_->addText3D(
        std::to_string(dis) + "m",
        pcl::PointXYZ((x + last_x_) / 2, (y + last_y_) / 2, (z + last_z_) / 2),
        0.5, 1.0, 0.0, 0.0, "LineText");
  } else {
    // clear buffer
    viewer_->removeText3D("LineText");
    viewer_->removeShape("PointLine");
    viewer_->removeShape("Point1");
    viewer_->removeShape("Point2");
    // set flag
    is_second_point_ = true;
    // store coordinate
    last_x_ = x;
    last_y_ = y;
    last_z_ = z;
    // tag point
    if (sphere_checkbox_->isChecked()) {
      viewer_->addSphere(pcl::PointXYZ(x, y, z), 0.05, 1.0, 0.0, 0.0, "Point1");
    }
  }
}