
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "utils/frame_info.h"

namespace autox {
namespace pointview {

FrameInfo::FrameInfo(std::shared_ptr<DeviceContext> device_context)
    : QObject(device_context->getDisplayContext()->getParent()) {
  property_tree_ =
      device_context->getPropertyTree()->createPropertySubTree("Info");
  // timestamp
  point_timestamp_label_ = std::make_shared<QLabel>();
  point_timestamp_label_->setText(QString::number(0));
  property_tree_->addProperty("timestamp", point_timestamp_label_);
  // points
  point_number_label_ = std::make_shared<QLabel>();
  point_number_label_->setText(QString::number(0));
  property_tree_->addProperty("points", point_number_label_);
  // fps
  point_fps_label_ = std::make_shared<QLabel>();
  point_fps_label_->setText(QString::number(0));
  property_tree_->addProperty("fps", point_fps_label_);
  point_play_fps_label_ = std::make_shared<QLabel>();
  point_play_fps_label_->setText(QString::number(0));
  property_tree_->addProperty("play fps", point_play_fps_label_);
}

void FrameInfo::update(double timestamp, size_t point_number) {
  point_number_label_->setText(QString::number(point_number));
  point_timestamp_label_->setText(QString::number(timestamp, 'f', 5));
  double dt = timestamp - last_time_;
  if (dt > 0.01 && dt < 1) {
    point_fps_label_->setText(QString::number(1.0 / dt, 'f', 5));
  }
  last_time_ = timestamp;
  TimePoint cur_system_time = std::chrono::steady_clock::now();
  double play_dt =
      std::chrono::duration<double>(cur_system_time - last_system_time_)
          .count();
  if (play_dt > 0.01) {
    point_play_fps_label_->setText(QString::number(1.0 / play_dt, 'f', 5));
  }
  last_system_time_ = cur_system_time;
}

}  // namespace pointview
}  // namespace autox
