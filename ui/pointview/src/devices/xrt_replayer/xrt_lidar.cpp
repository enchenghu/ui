
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt_lidar.h"

#include <QInputDialog>
#include <QMessageBox>
#include <iostream>

#include "xrt/proto/pointcloud.pb.h"

#include "xrt/record/record_message.h"

using namespace std::chrono_literals;

XrtLidar::XrtLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
                   autox::pointview::DeviceBaseParameter& parameter)
    : autox::pointview::DeviceBase(context, parameter),
      viewer_(context->getViewerPtr()) {
  // xrt message dispatcher
  dispatcher_ = XrtMessageDispatcher::getInstance();
  // create point cloud data
  cloud_.reset(new PointCloudT);
  swap_point_cloud_ = std::make_shared<RawPointCloud>();
  raw_point_cloud_ = std::make_shared<RawPointCloud>();
  // add point cloud to pcl viewer
  viewer_->addPointCloud(cloud_,
                         std::to_string(device_context_->getDeviceId()));
  // player
  device_context_->registerStartPlayerCb([this]() {
    channel_name_ = edit_channel_name_->text().toStdString();
    edit_channel_name_->setEnabled(false);
    btn_select_channel_->setEnabled(false);
    return true;
  });
  device_context_->registerPausePlayerCb([this]() {
    edit_channel_name_->setEnabled(true);
    btn_select_channel_->setEnabled(true);
    return true;
  });
  // point cloud info
  info_ = std::make_shared<autox::pointview::FrameInfo>(device_context_);
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);
  // point cloud manipulator
  std::vector<autox::pointview::ChannelSetting> channel_settings;
  channel_settings.push_back({"none", "gray", 0, 100});
  channel_settings.push_back({"x", "jet", -100, 100});
  channel_settings.push_back({"y", "jet", -100, 100});
  channel_settings.push_back({"z", "jet", -100, 100});
  channel_settings.push_back({"distance", "jet", 0, 100});
  channel_settings.push_back({"intensity", "jet", 0, 255});
  channel_settings.push_back({"confidence", "jet", 0, 255});
  channel_settings.push_back({"doppler", "bwr", -10, 10});
  channel_settings.push_back({"laser id", "rainbow", 1, 8});
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings);
  // point_selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // channel setting
  auto sub = device_context_->getPropertyTree()->createPropertySubTree(
      "channel setting");
  edit_channel_name_ = std::make_shared<QLineEdit>();
  edit_channel_name_->setText(QString::fromStdString(channel_name_));
  sub->addProperty("name", edit_channel_name_);
  btn_select_channel_ = std::make_shared<QPushButton>();
  btn_select_channel_->setText("Open");
  connect(btn_select_channel_.get(), &QPushButton::clicked, [this]() {
    // get lidar channel list
    QStringList channel_list;
    for (auto& channel : dispatcher_->getLidarChannelList()) {
      channel_list << QString::fromStdString(channel);
    }
    if (channel_list.size() == 0) {
      QMessageBox::warning(
          device_context_->getDisplayContext()->getParent(), "PointView",
          "no xrt channel for lidar, please check XrrReplayer!",
          QMessageBox::Ok);
      return;
    }
    // open dialog
    bool ok;
    QString selected_channel =
        QInputDialog::getItem(device_context_->getDisplayContext()->getParent(),
                              tr("Select Xrt Channel"), tr("Channel name:"),
                              channel_list, 0, false, &ok);
    if (!ok || selected_channel.isEmpty()) {
      return;
    }
    //
    edit_channel_name_->setText(selected_channel);
  });
  sub->addProperty("selection", btn_select_channel_);
  // save point cloud
  point_exporter_ = std::make_shared<autox::pointview::PointExporter>(
      device_context_, table_head_);
  point_exporter_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // create thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playing && !play_state.is_playback) {
        // get message from xrt buffer
        auto message = dispatcher_->get(channel_name_);
        if (message) {
          // publish msg
          if (!buffer_full_) {
            auto point_cloud = std::make_shared<RawPointCloud>();
            point_cloud->ParseFromString(message->content);
            if (point_cloud->x_size() != point_cloud->y_size() ||
                point_cloud->x_size() != point_cloud->z_size()) {
              LOG(INFO) << "invalid point cloud frame!";
              continue;
            }
            swap_point_cloud_ = point_cloud;
            buffer_full_ = true;
          } else {
            LOG(INFO) << "rendering too long, drop a point cloud frame!";
          }
        } else {
          std::this_thread::sleep_for(10ms);
        }
      } else {
        std::this_thread::sleep_for(100ms);
      }
    }
  });
}

XrtLidar::~XrtLidar() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

size_t XrtLidar::convertToPclPointCloud(int channel_idx) {
  size_t cnt = 0;
  for (int i = 0; i < raw_point_cloud_->x_size(); i++) {
    double v;
    std::uint8_t r, g, b;
    PointT p2;
    if (channel_idx == 0) {
      v = 100;
    } else if (channel_idx == 1) {
      // x
      v = raw_point_cloud_->x(i);
    } else if (channel_idx == 2) {
      // y
      v = raw_point_cloud_->y(i);
    } else if (channel_idx == 3) {
      // z
      v = raw_point_cloud_->z(i);
    } else if (channel_idx == 4) {
      // dis
      auto x = raw_point_cloud_->x(i);
      auto y = raw_point_cloud_->y(i);
      auto z = raw_point_cloud_->z(i);
      v = sqrt(x * x + y * y + z * z);
    } else if (channel_idx == 5 && i < raw_point_cloud_->intensity_size()) {
      // intensity
      v = raw_point_cloud_->intensity(i);
    } else if (channel_idx == 6 && i < raw_point_cloud_->confidence_size()) {
      // confidence
      v = raw_point_cloud_->confidence(i);
    } else if (channel_idx == 7 && i < raw_point_cloud_->doppler_size()) {
      // doppler
      v = raw_point_cloud_->doppler(i);
    } else if (channel_idx == 8 && i < raw_point_cloud_->laser_id_size()) {
      // laser id
      v = 1 + raw_point_cloud_->laser_id(i) % 8;
    } else {
      v = 0;
    }
    // v -> [0, 255]
    manipulator_->transformColor(v, r, g, b);
    p2.x = raw_point_cloud_->x(i);
    p2.y = raw_point_cloud_->y(i);
    p2.z = raw_point_cloud_->z(i);
    p2.r = r;
    p2.g = g;
    p2.b = b;
    cloud_->points[i] = p2;
    cnt++;
  }
  return cnt;
}

bool XrtLidar::updateUI() {
  {
    std::lock_guard<std::mutex>(this->lock);
    if (buffer_full_) {
      raw_point_cloud_ = swap_point_cloud_;
      device_context_->refreshPointCloud();
      buffer_full_ = false;
    }
  }
  if (!device_context_->getRefreshState()) {
    return false;
  }
  // assert
  assert(raw_point_cloud_->x_size() == raw_point_cloud_->y_size());
  assert(raw_point_cloud_->x_size() == raw_point_cloud_->z_size());
  auto start = std::chrono::steady_clock::now();
  cloud_->resize(raw_point_cloud_->x_size());
  auto end1 = std::chrono::steady_clock::now();
  // convert the raw point cloud (raw_point_cloud_) to pcl point cloud (cloud_)
  int channel_idx = manipulator_->getChannelIndex();
  size_t total_cnt = raw_point_cloud_->x_size();
  size_t valid_cnt = convertToPclPointCloud(channel_idx);
  auto end2 = std::chrono::steady_clock::now();
  pose_setting_->transform(cloud_);
  // set info
  info_->update(raw_point_cloud_->measurement_time(), valid_cnt);
  // update point selection
  point_selection_->update(cloud_);
  auto end3 = std::chrono::steady_clock::now();
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  // update point cloud
  // TODO(all) updatePointCloud will cause crash when the count of pointcloud
  // changed a lot, refer to https://github.com/PointCloudLibrary/pcl/pull/4017
  // (it's solved in pcl version1.11). another solution is using remove & add
  // instead of update pointcloud, it works well but not elegant. by the way,
  // for performace, remove & add API is faster than update API, which is
  // intresting.
  // viewer_->updatePointCloud(cloud_, cloud_id);
  viewer_->removePointCloud(cloud_id);
  viewer_->addPointCloud(cloud_, cloud_id);
  // update point hide/size
  manipulator_->update(cloud_id);
  point_exporter_->update(cloud_);
  // update flag
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  LOG(INFO) << "total point number: " << total_cnt << ", update time: " << dt
            << " ms";
  // debug info
  auto convert_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end2 - end1)
          .count();
  auto viewer_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - end3).count();
  LOG(INFO) << "convert time: " << convert_time
            << ", viewer time: " << viewer_time;
  return true;
}

bool XrtLidar::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= raw_point_cloud_->x_size()) {
    return false;
  }
  // required
  double x = raw_point_cloud_->x(idx);
  double y = raw_point_cloud_->y(idx);
  double z = raw_point_cloud_->z(idx);
  double distance = sqrt(x * x + y * y + z * z);
  // optional
  double laser_id = -1;
  if (idx < raw_point_cloud_->laser_id_size()) {
    laser_id = raw_point_cloud_->laser_id(idx);
  }
  double intensity = 0;
  if (idx < raw_point_cloud_->intensity_size()) {
    intensity = raw_point_cloud_->intensity(idx);
  }
  double timestamp = -1;
  if (idx < raw_point_cloud_->timestamp_size()) {
    timestamp = raw_point_cloud_->timestamp(idx);
  }
  double confidence = -1;
  if (idx < raw_point_cloud_->confidence_size()) {
    confidence = raw_point_cloud_->confidence(idx);
  }
  double doppler = 0;
  if (idx < raw_point_cloud_->doppler_size()) {
    doppler = raw_point_cloud_->doppler(idx);
  }
  data = std::vector<double>(
      {laser_id, x, y, z, distance, intensity, timestamp, confidence, doppler});
  return true;
}

bool XrtLidar::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                  config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  pose_setting_->initFromConfig(config);
  manipulator_->InitFromConfig(config);
  // channel_name
  std::string channel_name = "";
  config->getParameter("channel_name", channel_name);
  if (!channel_name.empty()) {
    edit_channel_name_->setText(QString::fromStdString(channel_name));
  }
  return true;
}

bool XrtLidar::storeToConfig(std::shared_ptr<autox::pointview::Config> config) {
  pose_setting_->storeToConfig(config);
  manipulator_->StoreToConfig(config);
  // channel_name
  auto channel_name = edit_channel_name_->text().toStdString();
  config->setParameter("channel_name", channel_name);
  return true;
}
