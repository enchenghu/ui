
#include "xradar.h"

#include <QMessageBox>
#include <iostream>

#include "driver/arbe_driver.h"

#define MAX_RECORDED_UDP_PACKETS (3600 * 10 * 600)  // 10min

using namespace std::chrono_literals;

XRadar::XRadar(std::shared_ptr<autox::pointview::DisplayContext> context,
               int device_id, const std::string& device_name)
    : autox::pointview::DeviceBase(context, device_id, device_name),
      viewer_(context->getViewerPtr()) {
  initDriver();
  // create point cloud data
  cloud_.reset(new PointCloudT);
  swap_point_cloud_ = std::make_shared<RawPointCloud>();
  raw_point_cloud_ = std::make_shared<RawPointCloud>();
  // add point cloud to pcl viewer
  viewer_->addPointCloud(cloud_,
                         std::to_string(device_context_->getDeviceId()));
  // point cloud info
  info_ = std::make_shared<autox::pointview::FrameInfo>(device_context_);
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);
  // point cloud manipulator
  std::vector<autox::pointview::ChannelSetting> channel_settings;
  channel_settings.push_back({"none", "gray", 0, 100});
  channel_settings.push_back({"x", "jet", 0, 100});
  channel_settings.push_back({"y", "jet", 0, 100});
  channel_settings.push_back({"z", "jet", 0, 100});
  channel_settings.push_back({"range", "jet", 0, 100});
  channel_settings.push_back({"doppler", "bwr", -10, 10});
  channel_settings.push_back({"power", "jet", 0, 255});
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings);
  // player setting
  player_setting_ = std::make_shared<autox::pointview::SimplePlayerSetting>(
      device_context_, udp_input_, pcap_parser_, playback_buffer_);
  player_setting_->setSyncPlayerCb([this]() { emit SyncPlayer(); });
  player_setting_->setResetDriverCb(
      [this](bool /*is_playback*/) { driver_->clear(); });
  // point selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // save point cloud
  point_exporter_ = std::make_shared<autox::pointview::PointExporter>(
      device_context_, table_head_);
  point_exporter_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // xradar setting
  xradar_setting_ =
      std::make_shared<autox::pointview::XRadarSetting>(device_context_);
}

XRadar::~XRadar() {
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void XRadar::initDriver() {
  // callback
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    driver_->processUdpPacket(data, len);
  };
  auto point_cloud_cb = [this](std::shared_ptr<RawPointCloud>& point_cloud) {
    std::lock_guard<std::mutex>(this->lock);
    if (device_context_->getPlayerState().is_playback) {
      // for pcap playback
      auto new_point_cloud = std::make_shared<RawPointCloud>();
      new_point_cloud.swap(point_cloud);
      playback_buffer_->addFrame(new_point_cloud->timestamp, new_point_cloud);
    } else {
      // for live streaming
      if (!buffer_full_) {
        swap_point_cloud_.swap(point_cloud);
        buffer_full_ = true;
      } else {
        std::cout << "rendering too long, drop a point cloud frame!"
                  << std::endl;
      }
    }
  };
  // driver
  driver_ = std::make_shared<autox::drivers::arbe::ArbeDriver>();
  driver_->setPointCloudCallback(point_cloud_cb);
  // for live streaming
  udp_input_ =
      std::make_shared<autox::pointview::UdpInput>(device_context_, true);
  udp_input_->setMaxRecordPacket(MAX_RECORDED_UDP_PACKETS);
  udp_input_->setUdpCallback(udp_cb);
  // for pcap playback
  pcap_parser_ =
      std::make_shared<autox::pointview::PcapUdpParser>(device_context_);
  pcap_parser_->setUdpCallback(udp_cb);
  playback_buffer_ = std::make_shared<PlaybackBuffer>(device_context_);
  playback_buffer_->setFrameCallback(
      [this](std::shared_ptr<RawPointCloud> data, int idx, double /*t*/) {
        std::lock_guard<std::mutex>(this->lock);
        if (!device_context_->getRefreshState()) {
          raw_point_cloud_ = data;
          device_context_->updateCurrentFrame(idx);
          device_context_->refreshPointCloud();
        }
      });
}

size_t XRadar::convertToPclPointCloud(size_t start, size_t end,
                                      int channel_idx) {
  double v;
  std::uint8_t r, g, b;
  PointT p2;
  size_t cnt = 0;
  for (size_t i = start; i < end; i++) {
    auto& p = raw_point_cloud_->points[i];
    bool valid = true;
    if (valid) {
      cnt++;
    }
    if (channel_idx == 0) {
      v = 100;
    } else if (channel_idx == 1) {
      // x
      v = fabs(p.x);
    } else if (channel_idx == 2) {
      // y
      v = fabs(p.y);
    } else if (channel_idx == 3) {
      // z
      v = fabs(p.z);
    } else if (channel_idx == 4) {
      // range
      v = p.range;
    } else if (channel_idx == 5) {
      // doppler
      v = p.doppler;
    } else if (channel_idx == 6) {
      // power
      v = p.power;
    }
    // v -> [0, 255]
    manipulator_->transformColor(v, r, g, b);
    p2.x = p.x;
    p2.y = p.y;
    p2.z = p.z;
    p2.r = r;
    p2.g = g;
    p2.b = b;
    if (valid) {
      p2.a = 255;
    } else {
      p2.a = 0;
    }
    cloud_->points[i] = p2;
  }
  return cnt;
}

bool XRadar::updateUI() {
  {
    std::lock_guard<std::mutex>(this->lock);
    if (buffer_full_) {
      raw_point_cloud_.swap(swap_point_cloud_);
      device_context_->refreshPointCloud();
      buffer_full_ = false;
    }
  }
  if (!device_context_->getRefreshState()) {
    return false;
  }
  auto start = std::chrono::steady_clock::now();
  cloud_->resize(raw_point_cloud_->points.size());
  // convert the raw point cloud (raw_point_cloud_) to pcl point cloud (cloud_)
  int channel_idx = manipulator_->getChannelIndex();
  size_t total_cnt = raw_point_cloud_->points.size();
  // convert by single thread
  size_t valid_cnt = convertToPclPointCloud(0, total_cnt, channel_idx);
  pose_setting_->transform(cloud_);
  // set info
  info_->update(raw_point_cloud_->timestamp, valid_cnt);
  // point selection update
  point_selection_->update(cloud_);
  // update viewer
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  viewer_->removePointCloud(cloud_id);
  viewer_->addPointCloud(cloud_, cloud_id);
  // update point hide/size
  manipulator_->update(cloud_id);
  point_exporter_->update(cloud_);
  // update flag
  device_context_->resetRefreshState();
  // debug info
  auto end = std::chrono::steady_clock::now();
  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  std::cout << "udp packet number: " << raw_point_cloud_->udp_packet_number
            << ", total point number: " << total_cnt
            << ", valid point number: " << valid_cnt << ", update time: " << dt
            << " ms" << std::endl;
  return true;
}

bool XRadar::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= raw_point_cloud_->points.size()) {
    return false;
  }
  auto& p = raw_point_cloud_->points[idx];
  double time_offset = p.timestamp - raw_point_cloud_->timestamp;
  data = std::vector<double>({p.elevation, p.azimuth, p.range, p.doppler,
                              p.power, time_offset, p.x, p.y, p.z});
  return true;
}

bool XRadar::initFromConfig(std::shared_ptr<autox::pointview::Config> config) {
  if (!config) {
    return false;
  }
  pose_setting_->initFromConfig(config);
  manipulator_->InitFromConfig(config);
  player_setting_->initFromConfig(config);
  xradar_setting_->initFromConfig(config);
  return true;
}

bool XRadar::storeToConfig(std::shared_ptr<autox::pointview::Config> config) {
  pose_setting_->storeToConfig(config);
  manipulator_->StoreToConfig(config);
  player_setting_->storeToConfig(config);
  xradar_setting_->storeToConfig(config);
  return true;
}

bool XRadar::updatePlayerState(autox::pointview::PlayerCmd cmd) {
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
