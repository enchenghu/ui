#include "hesai_qt64.h"

#include "driver/driver_qt64.h"

HesaiQt64::HesaiQt64(std::shared_ptr<autox::pointview::DisplayContext> context,
                     autox::pointview::DeviceBaseParameter& parameter)
    : autox::pointview::DeviceBase(context, parameter),
      viewer_(context->getViewerPtr()) {
  initDriver();
  current_frame_.pcl_pointcloud.reset(new PointCloudT);
  // add point cloud to pcl viewer
  viewer_->addPointCloud(current_frame_.pcl_pointcloud,
                         std::to_string(device_context_->getDeviceId()));
  // point cloud info
  info_ = std::make_shared<autox::pointview::LidarFrameInfo>(device_context_);
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);
  // point cloud manipulator
  std::vector<autox::pointview::ChannelSetting> channel_settings;
  channel_settings.push_back({"none", "gray", 0, 100});
  channel_settings.push_back({"x", "jet", 0, 100});
  channel_settings.push_back({"y", "jet", 0, 100});
  channel_settings.push_back({"z", "jet", 0, 100});
  channel_settings.push_back({"distance", "jet", 0, 100});
  channel_settings.push_back({"intensity", "jet", 0, 255});
  channel_settings.push_back({"laser id", "rainbow", 1, 8});
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings);

  // player setting
  player_setting_ = std::make_shared<autox::pointview::SimplePlayerSetting>(
      device_context_, udp_input_, pcap_parser_, playback_buffer_);
  player_setting_->setSyncPlayerCb([this]() { emit SyncPlayer(); });
  player_setting_->setResetDriverCb([this](bool /*is_playback*/) {
    driver_->resetFrameIndex();
    driver_->resetDriver();
  });
  // point_selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  point_selection_->setPointUidCallback(
      [this](size_t idx, int& uid) { return getPointUID(idx, uid); });
  // laser_track
  laser_track_ = std::make_shared<autox::pointview::LaserTrack>(device_context_,
                                                                table_head_);
  laser_track_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  laser_track_->setPointCheckCallback([this](size_t i) {
    auto& p = current_frame_.raw_pointcloud->points[i];
    return laser_track_->checkPoint(p.laser_id + 1, p.azimuth);
  });
  // point_exporter
  point_exporter_ = std::make_shared<autox::pointview::PointExporter>(
      device_context_, table_head_);
  point_exporter_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // range image
  range_image_ =
      std::make_shared<autox::pointview::RangeImage>(device_context_, 600, 64);
  range_image_->SetPointToPixelCallback([this](size_t idx, int& x, int& y) {
    auto& p = current_frame_.raw_pointcloud->points[idx];
    x = p.azimuth / 0.6;
    y = p.laser_id;
    return true;
  });
  // point_filter
  autox::pointview::PointFilterSetting point_filter_setting;
  initPointFilterSetting(point_filter_setting);
  point_filter_ = std::make_shared<autox::pointview::PointFilter>(
      device_context_, point_filter_setting);
  // return mode
  std::vector<autox::pointview::LidarReturnSetting> return_settings;
  return_settings.push_back({0x33, {"first"}});
  return_settings.push_back({0x38, {"last"}});
  return_settings.push_back({0x3B, {"first", "last"}});
  return_mode_info_ = std::make_shared<autox::pointview::LidarReturnInfo>(
      device_context_, return_settings);
}

HesaiQt64::~HesaiQt64() {
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void HesaiQt64::initDriver() {
  // callback
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    driver_->ParseLidarPacket(data, len);
  };

  auto set_packet_index_cb = [this](size_t index) {
    driver_->setCurPacketIndex(index);
  };

  auto point_cloud_cb = [this](std::shared_ptr<RawPointCloud>& point_cloud,
                               long long frame_index) {
    if (device_context_->getPlayerState().is_playback) {
      // for pcap playback
      auto new_point_cloud = std::make_shared<RawPointCloud>();
      *new_point_cloud = *point_cloud;
      if (pcap_parser_->getParserState()) {
        int real_index = frame_index % playback_buffer_->getRTFrameNum();
        playback_buffer_->addFrame(real_index, new_point_cloud->timestamp,
                                   new_point_cloud);
      } else {
        playback_buffer_->addRTFrameNum();
      }
    } else {
      std::lock_guard<std::mutex> lock(raw_pointcloud_buffer_mutex_);
      // for live streaming
      raw_pointcloud_buffer_.push_back(point_cloud);
      if (raw_pointcloud_buffer_.size() > 2) {
        raw_pointcloud_buffer_.pop_front();
        LOG(INFO) << "[warning] rendering too long, drop raw point cloud!";
      }
    }
  };
  // driver
  driver_ = std::make_shared<autox::drivers::hesai_qt64::DriverQt64>();
  driver_->setPointCloudCallback(point_cloud_cb);
  // for live streaming
  udp_input_ =
      std::make_shared<autox::pointview::UdpInput>(device_context_, true);
  udp_input_->setMaxRecordPacket(3600 * 10 * 60 * 10);  // 10min
  udp_input_->setUdpCallback(udp_cb);
  // for pcap playback
  pcap_parser_ =
      std::make_shared<autox::pointview::PcapUdpParser>(device_context_);
  pcap_parser_->setUdpCallback(udp_cb);
  pcap_parser_->setPacketIndexCallback(set_packet_index_cb);
  playback_buffer_ = std::make_shared<PlaybackBuffer>(device_context_);
  playback_buffer_->setFrameCallback(
      [this](std::shared_ptr<RawPointCloud> data, int idx, double /*t*/) {
        std::lock_guard<std::mutex> lock(raw_pointcloud_buffer_mutex_);
        if (raw_pointcloud_buffer_.size() < 2) {
          raw_pointcloud_buffer_.push_back(data);
          device_context_->updateCurrentFrame(idx);
        } else {
          LOG(INFO) << "[warning] rendering too long, drop raw point cloud!";
        }
      });
}

void HesaiQt64::convertToPclPointCloud(Frame& frame) {
  int channel_idx = manipulator_->getChannelIndex();
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  for (size_t i = 0; i < frame.n_points; i++) {
    double v;
    uint8_t r, g, b;
    PointT p2;
    auto& p = raw_pointcloud->points[i];
    if (channel_idx == 0) {
      v = 100;
    } else if (channel_idx == 1) {
      // x
      v = p.x;
    } else if (channel_idx == 2) {
      // y
      v = p.y;
    } else if (channel_idx == 3) {
      // z
      v = p.z;
    } else if (channel_idx == 4) {
      // dis
      v = p.distance;
    } else if (channel_idx == 5) {
      // intensity
      v = p.intensity;
    } else if (channel_idx == 6) {
      // laser id
      v = 1 + p.laser_id % 8;
    }
    // v -> [0, 255]
    manipulator_->transformColor(v, r, g, b);
    p2.x = p.x;
    p2.y = p.y;
    p2.z = p.z;
    p2.r = r;
    p2.g = g;
    p2.b = b;
    bool valid = return_mode_info_->checkValid(p.return_id);
    if (valid) {
      p2.a = 255;
    } else {
      p2.a = 0;
    }
    pcl_pointcloud->points[i] = p2;
  }
}

bool HesaiQt64::updateUI() {
  {
    std::lock_guard<std::mutex> lock(raw_pointcloud_buffer_mutex_);
    if (!raw_pointcloud_buffer_.empty()) {
      current_frame_.raw_pointcloud = raw_pointcloud_buffer_.front();
      device_context_->refreshPointCloud();
      raw_pointcloud_buffer_.pop_front();
    }
  }
  if (!device_context_->getRefreshState()) {
    return false;
  }
  if (!current_frame_.raw_pointcloud) {
    return false;
  }
  auto start = std::chrono::steady_clock::now();
  auto& raw_pointcloud = current_frame_.raw_pointcloud;
  auto& pcl_pointcloud = current_frame_.pcl_pointcloud;
  // set info in frame
  current_frame_.timestamp = raw_pointcloud->timestamp;
  current_frame_.n_udp_packets = raw_pointcloud->udp_packet_number;
  current_frame_.n_points = raw_pointcloud->points.size();
  // convert raw_pointcloud to pcl_pointcloud
  pcl_pointcloud->resize(current_frame_.n_points);
  convertToPclPointCloud(current_frame_);
  // transform pointcloud
  pose_setting_->transform(pcl_pointcloud);
  // update point selection
  point_selection_->update(pcl_pointcloud);
  // update laser track
  laser_track_->update(pcl_pointcloud);
  // update point_filter
  point_filter_->update(pcl_pointcloud);
  // update Udp Index in Pcap
  if (device_context_->getPlayerState().is_playback)
    player_setting_->updateUdpIndexPcap(raw_pointcloud->begin_index_in_pacp,
                                        raw_pointcloud->end_index_in_pacp);
  // update udp speed
  player_setting_->refreshUdpSpeed();
  // update range image
  range_image_->Update(pcl_pointcloud);
  // update point cloud
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  // viewer_->updatePointCloud(pcl_pointcloud, cloud_id);
  viewer_->removePointCloud(cloud_id);
  viewer_->addPointCloud(pcl_pointcloud, cloud_id);
  // update point hide/size
  manipulator_->update(cloud_id);
  point_exporter_->update(pcl_pointcloud);
  // set return info
  return_mode_info_->update(raw_pointcloud->return_mode);
  // set info
  size_t cnt = 0;
  for (size_t i = 0; i < current_frame_.n_points; i++) {
    if (pcl_pointcloud->points[i].a != 0) {
      cnt++;
    }
  }
  current_frame_.n_valid_points = cnt;
  info_->update(current_frame_.timestamp, current_frame_.n_valid_points, 0,
                current_frame_.n_udp_packets, 3, 1);
  // update flag
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  // debug
  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  LOG(INFO) << "udp packet number: " << current_frame_.n_udp_packets
            << ", total point number: " << current_frame_.n_points
            << ", valid point number: " << current_frame_.n_valid_points
            << ", update time: " << dt << " ms";
  return true;
}

bool HesaiQt64::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  double time_offset =
      p.timestamp * 1e-9 - current_frame_.raw_pointcloud->timestamp;
  data = std::vector<double>({(double)p.laser_id + 1, p.encoder, p.elevation,
                              p.azimuth, p.distance, p.intensity, time_offset,
                              p.x, p.y, p.z});
  return true;
}

bool HesaiQt64::getPointUID(size_t idx, int& uid) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  uid = p.encoder * 10000 + p.laser_id * 10 + p.return_id;
  return true;
}

bool HesaiQt64::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                   config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  pose_setting_->initFromConfig(config);
  manipulator_->InitFromConfig(config);
  player_setting_->initFromConfig(config);
  laser_track_->initFromConfig(config);
  range_image_->InitFromConfig(config);
  point_filter_->initFromConfig(config);
  return true;
}

bool HesaiQt64::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  pose_setting_->storeToConfig(config);
  manipulator_->StoreToConfig(config);
  player_setting_->storeToConfig(config);
  laser_track_->storeToConfig(config);
  range_image_->StoreToConfig(config);
  point_filter_->storeToConfig(config);
  return true;
}

void HesaiQt64::initPointFilterSetting(
    autox::pointview::PointFilterSetting& point_filter_setting) {
  point_filter_setting.addIntItem(
      "laser id", "", 1, 128,
      [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.laser_id + 1;
        return true;
      },
      true);
  point_filter_setting.addDoubleItem(
      "encoder", "deg", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.encoder;
        return true;
      });
  point_filter_setting.addDoubleItem(
      "azimuth", "deg", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.azimuth;
        return true;
      });
  point_filter_setting.addDoubleItem(
      "distance", "m", 0, 500, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.distance;
        return true;
      });
  point_filter_setting.addIntItem(
      "intensity", "", 0, 255, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.intensity;
        return true;
      });
}

bool HesaiQt64::updatePlayerState(autox::pointview::PlayerCmd cmd) {
  auto player_state = device_context_->getPlayerState();
  if (cmd.type == autox::pointview::PlayerCmdType::StartPlayer) {
    // start player
    if (player_state.is_playing) {
      return false;
    }
    if (!player_setting_->startPlayer()) return false;
  }
  return DeviceBase::updatePlayerState(cmd);
}
