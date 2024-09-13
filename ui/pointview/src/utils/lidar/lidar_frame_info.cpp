
/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "lidar_frame_info.h"

namespace autox {
namespace pointview {

LidarFrameInfo::LidarFrameInfo(std::shared_ptr<DeviceContext> device_context)
    : FrameInfo(device_context) {
  // return num
  label_return_num_ = std::make_shared<QLabel>();
  label_return_num_->setText(QString::number(0));
  property_tree_->addProperty("return num", label_return_num_);
  // udp packets
  label_packet_num_ = std::make_shared<QLabel>();
  label_packet_num_->setText(QString::number(0));
  property_tree_->addProperty("udp packets", label_packet_num_);
  // protocol version
  label_protocol_version_ = std::make_shared<QLabel>();
  label_protocol_version_->setText("0.0");
  property_tree_->addProperty("udp protocol", label_protocol_version_);
}

void LidarFrameInfo::update(double timestamp, size_t point_num,
                            size_t return_num, size_t packet_num,
                            int major_version, int minor_version) {
  FrameInfo::update(timestamp, point_num);
  label_return_num_->setText(QString::number(return_num));
  label_packet_num_->setText(QString::number(packet_num));
  if (major_protocol_version_ != major_version ||
      minor_protocol_version_ != minor_version) {
    auto protocol_version_str =
        QString::asprintf("%d.%d", major_version, minor_version);
    label_protocol_version_->setText(protocol_version_str);
    major_protocol_version_ = major_version;
    minor_protocol_version_ = minor_version;
  }
}

}  // namespace pointview
}  // namespace autox
