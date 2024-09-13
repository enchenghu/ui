/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"

namespace autox {
namespace pointview {

class FrameInfo : public QObject {
  Q_OBJECT

 public:
  FrameInfo(std::shared_ptr<DeviceContext> device_context);
  ~FrameInfo() = default;

 public:
  void update(double timestamp, size_t point_number);

 protected:
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
  std::shared_ptr<PropertyTree> property_tree_;
  // property widgets
  std::shared_ptr<QLabel> point_timestamp_label_;
  std::shared_ptr<QLabel> point_number_label_;
  std::shared_ptr<QLabel> point_fps_label_;
  std::shared_ptr<QLabel> point_play_fps_label_;
  // data
  double last_time_{0};
  TimePoint last_system_time_;
};

}  // namespace pointview
}  // namespace autox

#endif  // FRAME_INFO_H
