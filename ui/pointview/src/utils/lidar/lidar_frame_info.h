/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef LIDAR_FRAME_INFO_H
#define LIDAR_FRAME_INFO_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <memory>
#include <string>
#include <vector>

#include "utils/frame_info.h"

namespace autox {
namespace pointview {

class LidarFrameInfo : public FrameInfo {
  Q_OBJECT

 public:
  LidarFrameInfo(std::shared_ptr<DeviceContext> device_context);
  ~LidarFrameInfo() = default;

 public:
  void update(double timestamp, size_t point_num, size_t return_num,
              size_t packet_num, int major_version, int minor_version);

 private:
  // property widgets
  std::shared_ptr<QLabel> label_return_num_;
  std::shared_ptr<QLabel> label_packet_num_;
  std::shared_ptr<QLabel> label_protocol_version_;
  // data
  int major_protocol_version_{0};
  int minor_protocol_version_{0};
};

}  // namespace pointview
}  // namespace autox

#endif  // LIDAR_FRAME_INFO_H
