/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 5/4/23.
//

#ifndef POINTVIEW_POINT_DISTANCE_MEASURE_H
#define POINTVIEW_POINT_DISTANCE_MEASURE_H

#include <QCheckBox>

#include "utils/common/tool_base.h"

// point pick
class PointDistanceMeasure : public autox::pointview::ToolBase {
 public:
  explicit PointDistanceMeasure(
      std::shared_ptr<autox::pointview::DisplayContext> context);
  ~PointDistanceMeasure() override;

 protected:
  void PointPickCallback(const pcl::visualization::PointPickingEvent& event,
                         void* args);

 private:
  float last_x_, last_y_, last_z_;
  bool is_second_point_{false};
  std::shared_ptr<QCheckBox> sphere_checkbox_;
};

#endif  // POINTVIEW_POINT_DISTANCE_MEASURE_H
