
#include "xradar_2d.h"

#include <omp.h>

#include <QMessageBox>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "driver/xradar_2d_driver.h"
#include "utils/utils.h"

#define MAX_RECORDED_UDP_PACKETS (20000 * 10 * 30)  // 30s

using namespace std::chrono_literals;

namespace autox {
namespace drivers {
namespace xradar_2d {
XRadar2D::XRadar2D(std::shared_ptr<autox::pointview::DisplayContext> context,
                   int device_id, const std::string& device_name)
    : LidarBase<RawPointCloud>(context, device_id, device_name),
      viewer_(context->getViewerPtr()) {
  // point cloud manipulator
  channel_settings_.push_back({"range_code", "jet", 0, 255});
  channel_settings_.push_back({"doppler_code", "jet", 0, 255});
  channel_settings_.push_back({"azimuth_code", "rainbow", 1, 8});
  channel_settings_.push_back({"log_power", "rainbow", 1, 4});
  channel_settings_.push_back({"doppler", "jet", -5, 5});
  // lidar base init
  InitLidarBase();
  initDriver();
  // add point cloud to pcl viewer
  resetFrame(current_frame_);
  viewer_->addPointCloud(current_frame_.pcl_pointcloud,
                         std::to_string(device_context_->getDeviceId()));
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);
  // player setting
  player_setting_->setSyncPlayerCb([this]() { emit SyncPlayer(); });
  player_setting_->setResetDriverCb(
      [this](bool is_playback) { driver_->resetDriver(); });
  player_setting_->setUdpPort("10086");
  // point_selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // point_data
  point_data_ = std::make_shared<autox::pointview::PointData>(device_context_,
                                                              table_head_);
  point_data_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // video capture
  video_capture_ =
      std::make_shared<autox::pointview::VideoCapture>(device_context_);
  // save point cloud
  point_exporter_ = std::make_shared<autox::pointview::PointExporter>(
      device_context_, table_head_);
  point_exporter_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // point_filter
  autox::pointview::PointFilterSetting point_filter_setting;
  initPointFilterSetting(point_filter_setting);
  point_filter_ = std::make_shared<autox::pointview::PointFilter>(
      device_context_, point_filter_setting);
  // init rf image
  rf_viewer_.reset(new autox::pointview::rf_image::RfImage(
      device_context_, kXRadar2DRangeBins, kXRadar2DAzimuthBins, manipulator_));
}

XRadar2D::~XRadar2D() {
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void XRadar2D::initDriver() {
  // callback
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    driver_->parseRadarPacket(data, len);
  };

  auto set_packet_index_cb = [this](size_t index) {
    driver_->setCurPacketIndex(index);
  };

  auto set_packet_offset_cb = [this](long int offset) {
    driver_->setCurPacketOffset(offset);
  };

  auto reset_cb = [this](void) { driver_->resetDriver(); };

  auto point_cloud_cb = [this](std::shared_ptr<RawPointCloud> point_cloud,
                               long long frame_index) {
    if (device_context_->getPlayerState().is_playback) {
      // for pcap playback
      static bool write_end = true;
      auto new_point_cloud = std::make_shared<RawPointCloud>();
      *new_point_cloud = *point_cloud;
      if (pcap_parser_->getParserState()) {
        if (!write_end) {
          pcap_parser_->writeFileEnd();
          write_end = true;
        }
        int find_index =
            pcap_parser_->findFrameIndexInMap(new_point_cloud->offset_in_pcap);
        if (find_index > 0) {
          playback_buffer_->addFrame(find_index - 1, new_point_cloud->timestamp,
                                     new_point_cloud);
        } else {
          std::cout << "can't find frame in map,  offset_in_pcap is "
                    << new_point_cloud->offset_in_pcap << std::endl;
        }
      } else {
        write_end = false;
        playback_buffer_->addRTFrameNum();
        pcap_parser_->setFileOffsetMap(frame_index + 1,
                                       new_point_cloud->offset_in_pcap);
        pcap_parser_->setPcapIndexMap(frame_index + 1,
                                      new_point_cloud->begin_index_in_pacp - 1);
      }
    } else {
      // for live streaming
      // TODO(all): crash when set size > 1 due to bug in driver
      std::lock_guard<std::mutex> lock(raw_pointcloud_mutex_);
      raw_pointcloud_buffer_.push_back(point_cloud);
      if (raw_pointcloud_buffer_.size() > 2) {
        raw_pointcloud_buffer_.pop_front();
        std::cout << "[warning] converting too long, drop raw point cloud!"
                  << std::endl;
      }
    }
  };
  // create driver

  driver_ = std::make_shared<autox::drivers::xradar_2d::XRadar2DDriver>();
  driver_->setPointCloudCallback(point_cloud_cb);
  // for live streaming
  udp_input_->setMaxRecordPacket(MAX_RECORDED_UDP_PACKETS);
  udp_input_->setUdpCallback(udp_cb);
  // for pcap playback
  pcap_parser_->setUdpCallback(udp_cb);
  pcap_parser_->setResetDriverCallback(reset_cb);
  pcap_parser_->setPacketIndexCallback(set_packet_index_cb);
  pcap_parser_->setPacketOffsetCallback(set_packet_offset_cb);
  playback_buffer_->setFrameCallback(
      [this](std::shared_ptr<RawPointCloud> data, int idx, double /*t*/) {
        std::lock_guard<std::mutex> lock(raw_pointcloud_mutex_);
        if (raw_pointcloud_buffer_.size() < 3) {
          raw_pointcloud_buffer_.push_back(data);
          device_context_->updateCurrentFrame(idx);
        } else {
          std::cout << "[warning] converting too long, drop raw point cloud!"
                    << std::endl;
        }
      });
}

void XRadar2D::resetFrame(Frame& frame) {
  frame.pcl_pointcloud.reset(new PointCloudT);
}

void XRadar2D::convertToPclPointCloud(Frame& frame) {
  int channel_idx = manipulator_->getChannelIndex();
  auto colormap = manipulator_->getColormap();
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  size_t num_points = frame.raw_pointcloud->points.size();
  for (size_t i = 0; i < num_points; i++) {
    double v;
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
      v = p.range_code;
    } else if (channel_idx == 6) {
      // elongation
      v = p.doppler_code;
    } else if (channel_idx == 7) {
      // laser id
      v = p.azimuth_code;
    } else if (channel_idx == 8) {
      // return id
      v = p.log_power;
    } else if (channel_idx == 9) {
      v = p.doppler;
    }
    // v -> rgba[255,255,255,255]
    uint32_t rgba = colormap->get(v);
    p2.x = p.x;
    p2.y = p.y;
    p2.z = p.z;
    p2.r = (rgba >> 24) & 0xff;
    p2.g = (rgba >> 16) & 0xff;
    p2.b = (rgba >> 8) & 0xff;
    pcl_pointcloud->points[i] = p2;
  }
}

bool XRadar2D::updateUI() {
  {
    std::lock_guard<std::mutex> lock(raw_pointcloud_mutex_);
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
  auto& raw_pointcloud = current_frame_.raw_pointcloud;
  auto& pcl_pointcloud = current_frame_.pcl_pointcloud;
  // get info from raw_pointcloud
  current_frame_.timestamp = raw_pointcloud->timestamp;
  current_frame_.n_udp_packets = raw_pointcloud->udp_packet_number;
  current_frame_.n_points = raw_pointcloud->points.size();
  current_frame_.version = raw_pointcloud->version;
  // convert raw_pointcloud to pcl_pointcloud
  pcl_pointcloud->resize(current_frame_.n_points);
  convertToPclPointCloud(current_frame_);
  // transform
  pose_setting_->transform(pcl_pointcloud);
  // update point_filter_
  point_filter_->update(pcl_pointcloud);
  // mark for point_selection
  point_selection_->update(pcl_pointcloud);
  // point data
  point_data_->update(pcl_pointcloud);
  // export
  point_exporter_->update(pcl_pointcloud);

  float& range_resolution = current_frame_.raw_pointcloud->range_resolution;
  float& azimuth_resolution_by_deg =
      current_frame_.raw_pointcloud->azimuth_resolution;

  if (!rf_viewer_->IsInited()) {
    rf_viewer_->Init(
        current_frame_.raw_pointcloud->range_resolution,
        current_frame_.raw_pointcloud->azimuth_resolution * M_PI / 180.0);
  }
  rf_viewer_->Update(GenerateRfPointcloud());

  pcl::PointCloud<pcl::PointXYZRGBA> rf_pointcloud;
  auto rf_result = rf_viewer_->RequestRfByPointcloud(rf_pointcloud);

  // update udp speed
  player_setting_->refreshUdpSpeed();
  // update Udp Index in Pcap
  if (device_context_->getPlayerState().is_playback) {
    player_setting_->updateUdpIndexPcap(
        current_frame_.raw_pointcloud->begin_index_in_pacp,
        current_frame_.raw_pointcloud->end_index_in_pacp);
  }
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  viewer_->removePointCloud(cloud_id);
  if (rf_result) {
    viewer_->addPointCloud(rf_pointcloud.makeShared(), cloud_id);
  } else {
    viewer_->addPointCloud(current_frame_.pcl_pointcloud, cloud_id);
  }
  // update point hide/size
  manipulator_->update(cloud_id);
  size_t cnt = 0;
  for (size_t i = 0; i < current_frame_.n_points; i++) {
    if (pcl_pointcloud->points[i].a != 0) {
      cnt++;
    }
  }
  current_frame_.n_valid_points = cnt;
  // set info
  info_->update(current_frame_.timestamp, current_frame_.n_valid_points, 0,
                current_frame_.n_udp_packets, 4, current_frame_.version);
  Debug("Udp sequence first: " +
        std::to_string(current_frame_.raw_pointcloud->udp_sequences[0]));
  // update flag
  device_context_->resetRefreshState();
  return true;
}

bool XRadar2D::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  data = std::vector<double>({
      (double)p.range_code,
      (double)p.doppler_code,
      (double)p.azimuth_code,
      (double)p.log_power,
      p.timestamp,
      (double)p.distance,
      (double)p.detect_type,
      p.x,
      p.y,
      p.z,
      (double)p.azimuth,
      p.doppler,
  });
  return true;
}

bool XRadar2D::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                  config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  // init driver
  config->getParameter("driver.min_distance", min_distance_);
  config->getParameter("driver.max_distance", max_distance_);
  config->getParameter("driver.hfov_start", hfov_start_);
  config->getParameter("driver.hfov_end", hfov_end_);

  driver_->config(min_distance_, max_distance_, hfov_start_, hfov_end_);
  //
  pose_setting_->initFromConfig(config);
  point_filter_->initFromConfig(config);
  return LidarBase::initFromConfig(config);
}

bool XRadar2D::storeToConfig(std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter("driver.min_distance", min_distance_);
  config->setParameter("driver.max_distance", max_distance_);
  config->setParameter("driver.hfov_start", hfov_start_);
  config->setParameter("driver.hfov_end", hfov_end_);
  pose_setting_->storeToConfig(config);
  point_filter_->storeToConfig(config);
  return LidarBase::storeToConfig(config);
}

void XRadar2D::initPointFilterSetting(
    autox::pointview::PointFilterSetting& point_filter_setting) {
  point_filter_setting.addIntItem(
      "detect_type", "", 1, 5,
      [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.detect_type;
        return true;
      },
      true);
  point_filter_setting.addDoubleItem(
      "doppler", "m/s", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.doppler;
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
  point_filter_setting.addDoubleItem(
      "azimuth", "deg", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.azimuth;
        return true;
      });
}

bool XRadar2D::updatePlayerState(autox::pointview::PlayerCmd cmd) {
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

autox::pointview::rf_image::PointCloud XRadar2D::GenerateRfPointcloud() {
  autox::pointview::rf_image::PointCloud point_cloud;
  std::vector<autox::drivers::xradar_2d::Point>& raw_pointcloud =
      current_frame_.raw_pointcloud->points;
  point_cloud.points.reserve(raw_pointcloud.size());
  for (size_t i = 0; i < raw_pointcloud.size(); i++) {
    auto& p = raw_pointcloud[i];
    point_cloud.points.push_back(autox::pointview::rf_image::Point{
        p.range_code, p.doppler_code, p.azimuth_code, p.log_power, p.doppler});
  }
  point_cloud.timestamp = current_frame_.raw_pointcloud->timestamp;
  return point_cloud;
}
}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox