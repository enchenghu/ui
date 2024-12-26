/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 9/11/24.
//

#include "lidar_base.h"
namespace autox {
namespace pointview {
template <class RawPointCloud>
LidarBase<RawPointCloud>::LidarBase(
    std::shared_ptr<autox::pointview::DisplayContext> context,
    DeviceBaseParameter& parameter)
    : DeviceBase(context, parameter) {
  // point cloud manipulator
  channel_settings_.push_back({"none", "gray", 0, 100});
  channel_settings_.push_back({"x", "jet", -100, 100});
  channel_settings_.push_back({"y", "jet", -100, 100});
  channel_settings_.push_back({"z", "jet", -10, 10});
  channel_settings_.push_back({"distance", "jet", 0, 100});
}

template <class RawPointCloud>
LidarBase<RawPointCloud>::~LidarBase() {}

template <class RawPointCloud>
void LidarBase<RawPointCloud>::InitLidarBase() {
  // point cloud info
  info_ = std::make_shared<autox::pointview::LidarFrameInfo>(device_context_);
  // point cloud manipulator
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings_);
  // for live streaming
  udp_input_ =
      std::make_shared<autox::pointview::UdpInput>(device_context_, true, 1500);
  // for pcap playback
  pcap_parser_ =
      std::make_shared<autox::pointview::PcapUdpParser>(device_context_);
  playback_buffer_ = std::make_shared<PlaybackBuffer>(device_context_);
  // player setting
  player_setting_ = std::make_shared<autox::pointview::SimplePlayerSetting>(
      device_context_, udp_input_, pcap_parser_, playback_buffer_);
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);
  // video capture
  video_capture_ =
      std::make_shared<autox::pointview::VideoCapture>(device_context_);
  // video player
  video_player_ =
      std::make_shared<autox::pointview::VideoPlayer>(device_context_);
}

template <class RawPointCloud>
bool LidarBase<RawPointCloud>::initFromConfig(std::shared_ptr<Config> config) {
  manipulator_->InitFromConfig(config);
  player_setting_->initFromConfig(config);
  pose_setting_->initFromConfig(config);
  return true;
}

template <class RawPointCloud>
bool LidarBase<RawPointCloud>::storeToConfig(std::shared_ptr<Config> config) {
  manipulator_->StoreToConfig(config);
  player_setting_->storeToConfig(config);
  pose_setting_->storeToConfig(config);
  return true;
}

}  // namespace pointview
}  // namespace autox