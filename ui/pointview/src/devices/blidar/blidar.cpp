/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "blidar.h"

#include <QMessageBox>
#include <iostream>

#include "driver/blidar_driver.h"

using namespace std::chrono_literals;

namespace autox::drivers::blidar {
BLidar::BLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
               autox::pointview::DeviceBaseParameter& parameter)
    : LidarBase<RawPointCloud>(context, parameter),
      viewer_(context->getViewerPtr()) {
  // point cloud manipulator
  channel_settings_.push_back({"intensity", "jet", 0, 255});
  channel_settings_.push_back({"laser id", "rainbow", 1, 4});
  channel_settings_.push_back({"frame id", "rainbow", 1, 3});
  channel_settings_.push_back({"return id", "rainbow", 1, 4});
  channel_settings_.push_back({"elongation", "jet", 0, 255});
  // lidar base init
  InitLidarBase();
  initDriver();
  current_frame_.pcl_pointcloud.reset(new PointCloudT);
  // add point cloud to pcl viewer
  viewer_->addPointCloud(current_frame_.pcl_pointcloud,
                         std::to_string(device_context_->getDeviceId()));
  // adc plot
  adc_plot_ =
      std::make_shared<autox::pointview::BLidarAdcPlot>(device_context_);
  // player setting
  player_setting_->setSyncPlayerCb([this]() { emit SyncPlayer(); });
  player_setting_->setResetDriverCb([this](bool /*is_playback*/) {
    driver_->resetDriverLoad();
    driver_->resetDriver();
  });
  player_setting_->setUdpPort("2469");
  // point_selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  point_selection_->setPointUidCallback(
      [this](size_t idx, int& uid) { return getPointUID(idx, uid); });
  // point_data
  point_data_ = std::make_shared<autox::pointview::PointData>(device_context_,
                                                              table_head_);
  point_data_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // range image
  range_image_ = std::make_shared<autox::pointview::RangeImage>(
      device_context_, range_image_width_, range_image_height_);
  // lidar correction
  lidar_intrinsics_ =
      std::make_shared<autox::pointview::LidarIntrinsics>(device_context_, 4);
  lidar_intrinsics_->registerSetIntrinsicsCallback(
      [this](const std::vector<double>& elevation,
             const std::vector<double>& azimuth,
             const std::vector<double>& distance) {
        driver_->setIntrinsics(elevation, azimuth, distance);
        // reload pcap if playback
        autox::pointview::PlayerCmd cmd;
        cmd.type = autox::pointview::PlayerCmdType::ReloadPlayback;
        device_context_->updatePlayerState(cmd);
        return true;
      });
  lidar_intrinsics_->registerGetIntrinsicsCallback(
      [this](std::vector<double>& elevation, std::vector<double>& azimuth,
             std::vector<double>& distance) {
        driver_->getIntrinsics(elevation, azimuth, distance);
        return true;
      });
  lidar_intrinsics_->registerGetLaserIdCallback([this](size_t idx) {
    if (idx >= current_frame_.raw_pointcloud->points.size()) {
      return -1;
    }
    int laser_id = current_frame_.raw_pointcloud->points[idx].laser_id;
    return laser_id;
  });
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
  // debug
  auto debug_sub =
      device_context_->getPropertyTree()->createPropertySubTree("debug");
  checkbox_range_image_correction_ = std::make_shared<QCheckBox>();
  debug_sub->addProperty("Range Image correction",
                         checkbox_range_image_correction_);
  // signal and slots
  connect(checkbox_range_image_correction_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_range_image_correction_ = state > 0;
            device_context_->refreshPointCloud();
          });
  connect(range_image_.get(), SIGNAL(PickRangeImagePoint(int, int)),
          adc_plot_.get(), SLOT(set_position(int, int)));
  connect(adc_plot_.get(), SIGNAL(PostionValueChanged(int, int)),
          range_image_.get(), SLOT(set_position(int, int)));
  connect(adc_plot_.get(), SIGNAL(SelectMultiPoints(bool)), range_image_.get(),
          SLOT(set_select_status(bool)));
  connect(adc_plot_.get(), SIGNAL(SaveImage(QString)), range_image_.get(),
          SLOT(SaveImage(QString)));
}

BLidar::~BLidar() {
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void BLidar::initDriver() {
  // callback
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    driver_->ParseLidarPacket(data, len);
  };

  auto set_packet_index_cb = [this](size_t index) {
    driver_->setCurPacketIndex(index);
  };
  auto set_packet_offset_cb = [this](long int offset) {
    driver_->setCurPacketOffset(offset);
  };

  auto reset_cb = [this]() { driver_->resetDriver(); };

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
          LOG(INFO) << "can't find frame in map,  offset_in_pcap is "
                    << new_point_cloud->offset_in_pcap;
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
      LOG(INFO) << "Add point cloud to buffer";
      std::lock_guard<std::mutex> lock(raw_pointcloud_buffer_mutex_);
      // for live streaming
      raw_pointcloud_buffer_.push_back(point_cloud);
      if (device_context_->getPlayerState().is_recording) {
        device_context_->updateCurrentFrame(record_frame_index++);
      } else {
        record_frame_index = 0;
      }
      if (raw_pointcloud_buffer_.size() > 2) {
        raw_pointcloud_buffer_.pop_front();
        LOG(INFO) << "[warning] rendering too long, drop raw point cloud!";
      }
    }
  };
  // driver
  driver_ = std::make_shared<autox::drivers::blidar::BLidarDriver>();
  driver_->setPointCloudCallback(point_cloud_cb);
  // for live streaming
  udp_input_->setMaxRecordPacket(3600 * 10 * 60 * 10);  // 10min
  udp_input_->setUdpCallback(udp_cb);
  // for pcap playback
  pcap_parser_->setUdpCallback(udp_cb);
  pcap_parser_->setResetDriverCallback(reset_cb);
  pcap_parser_->setPacketIndexCallback(set_packet_index_cb);
  pcap_parser_->setPacketOffsetCallback(set_packet_offset_cb);
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

void BLidar::convertToPclPointCloud(Frame& frame) {
  int channel_idx = manipulator_->getChannelIndex();
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  for (size_t i = 0; i < frame.n_points; i++) {
    double v;
    std::uint8_t r, g, b;
    PointT p2;
    auto& p = raw_pointcloud->points[i];
    switch (channel_idx) {
      case 0: {
        v = 100;
        break;
      }
      case 1: {
        v = p.x;
        break;
      }
      case 2: {
        v = p.y;
        break;
      }
      case 3: {
        v = p.z;
        break;
      }
      case 4: {
        v = p.distance;
        break;
      }
      case 5: {
        v = p.intensity;
        break;
      }
      case 6: {
        v = 1 + p.laser_id;
        break;
      }
      case 7: {
        // frame id
        v = 1 + p.frame_id;
        break;
      }
      case 8: {
        v = p.return_id + 1;
        break;
      }
      case 9: {
        v = p.elongation;
        break;
      }
      default: {
        break;
      }
    }
    // v -> [0, 255]
    manipulator_->transformColor(v, r, g, b);
    p2.x = p.x;
    p2.y = p.y;
    p2.z = p.z;
    p2.r = r;
    p2.g = g;
    p2.b = b;
    pcl_pointcloud->points[i] = p2;
  }
}

bool BLidar::updateUI() {
  adc_plot_->Update();
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
  current_frame_.version = raw_pointcloud->version;
  current_frame_.max_return_num = raw_pointcloud->max_return_num;
  // convert raw_pointcloud to pcl_pointcloud
  pcl_pointcloud->resize(current_frame_.n_points);
  convertToPclPointCloud(current_frame_);
  // transform pointcloud
  pose_setting_->transform(pcl_pointcloud);
  // update point selection
  point_selection_->update(pcl_pointcloud);
  // point data
  point_data_->update(pcl_pointcloud);
  // update point_filter_
  point_filter_->update(pcl_pointcloud);
  // for intrinsics calibration
  lidar_intrinsics_->update(pcl_pointcloud);
  // update Udp Index in Pcap
  if (device_context_->getPlayerState().is_playback)
    player_setting_->updateUdpIndexPcap(raw_pointcloud->begin_index_in_pacp,
                                        raw_pointcloud->end_index_in_pacp);
  // update udp speed
  player_setting_->refreshUdpSpeed();
  // update point cloud
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  // TODO(all) updatePointCloud will cause crash when the count of pointcloud
  // changed a lot, refer to https://github.com/PointCloudLibrary/pcl/pull/4017
  // (it's solved in pcl version1.11). another solution is using remove & add
  // instead of update pointcloud, it works well but not elegant. by the way,
  // for performace, remove & add API is faster than update API, which is
  // intresting.
  // viewer_->updatePointCloud(cloud_, cloud_id);
  viewer_->removePointCloud(cloud_id);
  viewer_->addPointCloud(pcl_pointcloud, cloud_id);
  // update point hide/size
  manipulator_->update(cloud_id);
  point_exporter_->update(pcl_pointcloud);
  // set info
  size_t cnt = 0;
  for (size_t i = 0; i < current_frame_.n_points; i++) {
    if (pcl_pointcloud->points[i].a != 0) {
      cnt++;
    }
  }
  current_frame_.n_valid_points = cnt;
  info_->update(current_frame_.timestamp, current_frame_.n_valid_points,
                current_frame_.max_return_num, current_frame_.n_udp_packets, 5,
                current_frame_.version);
  // update range image
  convertToRangeImage(current_frame_);
  range_image_->Update(current_frame_.range_image);
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

bool BLidar::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  data.clear();
  data.push_back(p.laser_id + 1);
  data.push_back(p.return_id + 1);
  data.push_back(p.frame_id + 1);
  data.push_back(p.elevation_encoder);
  data.push_back(p.azimuth_encoder);
  data.push_back(p.elevation);
  data.push_back(p.azimuth);
  data.push_back(p.distance);
  data.push_back(p.intensity);
  data.push_back(p.elongation);
  data.push_back(p.timestamp);
  data.push_back(p.x);
  data.push_back(p.y);
  data.push_back(p.z);
  return true;
}

bool BLidar::getPointUID(size_t idx, int& uid) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  uid = p.azimuth_encoder * 100 + p.laser_id * 10 + p.return_id;
  return true;
}

bool BLidar::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  point_filter_->initFromConfig(config);
  lidar_intrinsics_->initFromConfig(config);
  range_image_->InitFromConfig(config);
  adc_plot_->InitFromConfig(config);
  return LidarBase::initFromConfig(config);
}

bool BLidar::storeToConfig(std::shared_ptr<autox::pointview::Config> config) {
  point_filter_->storeToConfig(config);
  lidar_intrinsics_->storeToConfig(config);
  range_image_->StoreToConfig(config);
  adc_plot_->StoreToConfig(config);
  return LidarBase::storeToConfig(config);
}

void BLidar::initPointFilterSetting(
    autox::pointview::PointFilterSetting& point_filter_setting) {
  point_filter_setting.addIntItem(
      "laser id", "", 1, 4,
      [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.laser_id + 1;
        return true;
      },
      true);
  point_filter_setting.addIntItem(
      "frame id", "", 1, 3, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.frame_id + 1;
        return true;
      });
  point_filter_setting.addIntItem(
      "return id", "", 1, 8, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.return_id + 1;
        return true;
      });
  point_filter_setting.addIntItem(
      "azimuth encoder", "", 0, 3000, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.azimuth_encoder;
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
  point_filter_setting.addIntItem(
      "elevation encoder", "", 0, 366, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.elevation_encoder;
        return true;
      });
  point_filter_setting.addDoubleItem(
      "elevation", "deg", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.elevation;
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

bool BLidar::updatePlayerState(autox::pointview::PlayerCmd cmd) {
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

void BLidar::convertToRangeImage(Frame& frame) {
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  auto& image = frame.range_image;
  size_t num_points = raw_pointcloud->points.size();
  // init range image
  image = QImage(range_image_width_, range_image_height_, QImage::Format_RGB32);
  image.fill(0);
  std::vector<QRgb*> image_row_ptr;
  image_row_ptr.resize(image.height());
  for (int i = 0; i < image.height(); i++) {
    image_row_ptr[i] = reinterpret_cast<QRgb*>(image.scanLine(i));
  }
  for (size_t i = 0; i < num_points; i++) {
    auto& p = raw_pointcloud->points[i];
    auto& p2 = pcl_pointcloud->points[i];
    // range image
    if (p2.a != 0) {
      int range_image_x;
      if (enable_range_image_correction_) {
        range_image_x =
            (int)(atan2(p.x, p.y) * range_image_width_ / (2 * M_PI));
        if (range_image_x < 0) {
          range_image_x += range_image_width_;
        }
        range_image_x = std::clamp(range_image_x, 0, range_image_width_ - 1);
      } else {
        range_image_x = p.scan_column;
      }
      int range_image_y = p.scan_row;
      if (range_image_x > range_image_width_ - 1 || range_image_x < 0) {
        LOG(INFO) << "x out of range: " << range_image_x;
        continue;
      }
      if (range_image_y > range_image_height_ - 1 || range_image_x < 0) {
        LOG(INFO) << "y out of range: " << range_image_y;
        continue;
      }
      auto range_image_color = qRgb(uint(p2.r), uint(p2.g), uint(p2.b));
      QRgb* row = image_row_ptr[range_image_y];
      row[range_image_x] = range_image_color;
    }
  }
}
}  // namespace autox::drivers::blidar