/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 9/11/24.
//

#ifndef POINTVIEW_LIDAR_BASE_H
#define POINTVIEW_LIDAR_BASE_H

#include <memory>

#include "utils/camera/video_capture.h"
#include "utils/camera/video_player.h"
#include "utils/common/device_base.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/simple_player_setting.h"
#include "utils/io/udp_input.h"
#include "utils/lidar/lidar_frame_info.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pose_setting.h"

namespace autox {
namespace pointview {

template <class RawPointCloud>
class LidarBase : public DeviceBase {
  using PlaybackBuffer = autox::pointview::PlaybackBuffer<RawPointCloud>;

 public:
  LidarBase(std::shared_ptr<autox::pointview::DisplayContext> context,
            DeviceBaseParameter& parameter);
  ~LidarBase();
  void InitLidarBase();
  // config
  bool initFromConfig(std::shared_ptr<Config> config) override;
  bool storeToConfig(std::shared_ptr<Config> config) override;

 protected:
  // helper utils
  std::shared_ptr<autox::pointview::LidarFrameInfo> info_;
  std::vector<autox::pointview::ChannelSetting> channel_settings_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  std::shared_ptr<autox::pointview::SimplePlayerSetting> player_setting_;
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  // for live streaming
  std::shared_ptr<autox::pointview::UdpInput> udp_input_;
  // for pcap playback
  std::shared_ptr<autox::pointview::PcapUdpParser> pcap_parser_;
  std::shared_ptr<PlaybackBuffer> playback_buffer_;
  // video
  std::shared_ptr<autox::pointview::VideoCapture> video_capture_;
  std::shared_ptr<autox::pointview::VideoPlayer> video_player_;
};

}  // namespace pointview
}  // namespace autox

#include "utils/common/lidar_base.hpp"
#endif  // POINTVIEW_LIDAR_BASE_H
