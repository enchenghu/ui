#include "hesai_ft120.h"

#include "hesai_lidar_sdk.hpp"
#include "utils/utils.h"

HesaiFT120::HesaiFT120(
    std::shared_ptr<autox::pointview::DisplayContext> context,
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
  channel_settings.push_back({"intensity", "jet", 0, 50});
  channel_settings.push_back({"distance", "jet", 0, 10});
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings);
  // point_selection
  point_selection_ = std::make_shared<autox::pointview::PointSelection>(
      device_context_, table_head_);
  point_selection_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  point_selection_->setPointUidCallback(
      [this](size_t idx, int& uid) { return getPointUID(idx, uid); });
  // point_exporter
  point_exporter_ = std::make_shared<autox::pointview::PointExporter>(
      device_context_, table_head_);
  point_exporter_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  // create property
  auto root_tree = device_context_->getPropertyTree();
  // combobox_player_type_
  combobox_player_type_ = std::make_shared<QComboBox>();
  combobox_player_type_->addItems({"UDP", "PCAP"});
  connect(combobox_player_type_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(playerTypeChanged(int)));
  root_tree->addProperty("type", combobox_player_type_);
  edit_udp_port_ = std::make_shared<QLineEdit>();
  edit_udp_port_->setText("2368");
  root_tree->addProperty("udp port", edit_udp_port_);
  edit_udp_ip_ = std::make_shared<QLineEdit>();
  edit_udp_ip_->setText("192.168.1.201");
  root_tree->addProperty("lidar ip", edit_udp_ip_);
  // pcap file
  btn_pcap_file_ = std::make_shared<QPushButton>();
  btn_pcap_file_->setText("Open");
  root_tree->addProperty("pcap file", btn_pcap_file_);
  connect(btn_pcap_file_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openPcapClicked()));
  // pcap file status
  label_pcap_file_ = std::make_shared<QLabel>();
  label_pcap_file_->setText("no pcap file");
  label_pcap_file_->setAlignment(Qt::AlignRight);
  root_tree->addProperty("pcap file status", label_pcap_file_);
  last_pcap_open_dirpath_ = QDir::homePath();
  crt_pcap_path_ = "";
  // correction file
  correction_file_button_ = std::make_shared<QPushButton>();
  correction_file_button_->setText("Open");
  root_tree->addProperty("correction file", correction_file_button_);
  correction_file_path_ = "";
  connect(correction_file_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openCorrectionFileClicked()));
  label_correction_file_ = std::make_shared<QLabel>();
  label_correction_file_->setText("no correction file");
  label_correction_file_->setAlignment(Qt::AlignRight);
  root_tree->addProperty("correction file status", label_correction_file_);
  // set player callback
  device_context_->registerPausePlayerCb([this]() {
    combobox_player_type_->setEnabled(true);
    auto play_state = device_context_->getPlayerState();
    edit_udp_port_->setEnabled(!play_state.is_playback);
    edit_udp_ip_->setEnabled(!play_state.is_playback);
    btn_pcap_file_->setEnabled(play_state.is_playback);
    if (!play_state.is_playback) {
      hesai_sdk_->Stop();
    }
    return true;
  });
}

HesaiFT120::~HesaiFT120() {
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void HesaiFT120::initDriver() {
  // callback
  auto point_cloud_cb = [this](const LidarDecodedFrame<PointXYZIT>& frame) {
    std::shared_ptr<RawPointCloud> pcl_pointcloud(new RawPointCloud);
    if (frame.points_num == 0) return;
    pcl_pointcloud->clear();
    pcl_pointcloud->resize(frame.points_num);
    pcl_pointcloud->points.assign(frame.points,
                                  frame.points + frame.points_num);
    pcl_pointcloud->height = 1;
    pcl_pointcloud->width = frame.points_num;
    pcl_pointcloud->is_dense = false;
    std::lock_guard<std::mutex> lock(raw_pointcloud_buffer_mutex_);
    raw_pointcloud_buffer_.push_back(pcl_pointcloud);
    if (raw_pointcloud_buffer_.size() > 2) {
      raw_pointcloud_buffer_.pop_front();
      std::cout << "[warning] rendering too long, drop raw point cloud!"
                << std::endl;
    }
  };
  // driver
  hesai_sdk_ = std::make_shared<hesai::lidar::HesaiLidarSdk<PointXYZIT>>();
  hesai_sdk_->RegRecvCallback(point_cloud_cb);
}

void HesaiFT120::convertToPclPointCloud(Frame& frame) {
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
      // intensity
      v = p.intensity;
    } else if (channel_idx == 5) {
      v = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
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

bool HesaiFT120::updateUI() {
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
  current_frame_.timestamp = raw_pointcloud->points.back().timestamp;
  current_frame_.n_udp_packets = 0;
  current_frame_.n_points = raw_pointcloud->points.size();
  // convert raw_pointcloud to pcl_pointcloud
  pcl_pointcloud->resize(current_frame_.n_points);
  convertToPclPointCloud(current_frame_);
  // transform pointcloud
  pose_setting_->transform(pcl_pointcloud);
  // update point selection
  point_selection_->update(pcl_pointcloud);
  // update point cloud
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  // viewer_->updatePointCloud(pcl_pointcloud, cloud_id);
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
  info_->update(current_frame_.timestamp, current_frame_.n_valid_points, 0,
                current_frame_.n_udp_packets, 1, 4);
  // update flag
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  // debug
  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  std::cout << "udp packet number: " << current_frame_.n_udp_packets
            << ", total point number: " << current_frame_.n_points
            << ", valid point number: " << current_frame_.n_valid_points
            << ", update time: " << dt << " ms" << std::endl;
  return true;
}

bool HesaiFT120::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  data = std::vector<double>({p.x, p.y, p.z});
  return true;
}

bool HesaiFT120::getPointUID(size_t idx, int& uid) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  uid = p.x * 10000 + p.y * 10 + p.z;
  return true;
}

bool HesaiFT120::openPcapClicked() {
  // choose pcap with dialog
  QString pcap_file = GetOpenFileName("Open Pcap File", last_pcap_open_dirpath_,
                                      "Pcap File(*.pcap);;All Files(*.*)");
  if (pcap_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    label_pcap_file_->setText("no pcap file");
    return false;
  }
  last_pcap_open_dirpath_ = QFileInfo(pcap_file).dir().absolutePath();
  crt_pcap_path_ = pcap_file.toStdString();
  label_pcap_file_->setText(pcap_file);
  return true;
}

void HesaiFT120::playerTypeChanged(int index) {
  bool is_playback = (index == 1);
  device_context_->updatePlayerType(is_playback);
  edit_udp_port_->setEnabled(!is_playback);
  edit_udp_ip_->setEnabled(!is_playback);
  btn_pcap_file_->setEnabled(is_playback);
}

bool HesaiFT120::updatePlayerState(autox::pointview::PlayerCmd cmd) {
  auto player_state = device_context_->getPlayerState();
  if (cmd.type == autox::pointview::PlayerCmdType::StartPlayer) {
    // start player
    if (player_state.is_playing) {
      return false;
    }
    DriverParam parameter;
    parameter.input_param.correction_file_path = correction_file_path_;
    if (player_state.is_playback) {
      parameter.input_param.source_type = DATA_FROM_PCAP;
      parameter.input_param.pcap_path = crt_pcap_path_;
    } else {
      std::string port = edit_udp_port_->text().toStdString();
      std::string ip = edit_udp_ip_->text().toStdString();
      parameter.input_param.source_type = DATA_FROM_LIDAR;
      parameter.input_param.device_ip_address =
          ip;  // 192.168.1.201 is the lidar ip address
      parameter.input_param.ptc_port = 9347;  // 9347 is the lidar ptc port
      parameter.input_param.udp_port =
          std::stoi(port);  // 2368 is the lidar udp port
      parameter.input_param.host_ip_address =
          "192.168.1.1";  // 192.168.1.100 is the pc ip address
      parameter.input_param.multicast_ip_address =
          "";  // 239.0.0.1 is the lidar multcast ip address, set
               // this parameter to "" when lidar do not support
               // muticast
    }
    hesai_sdk_->Init(parameter);
    hesai_sdk_->Start();
  }
  combobox_player_type_->setEnabled(false);
  edit_udp_port_->setEnabled(false);
  edit_udp_ip_->setEnabled(false);
  btn_pcap_file_->setEnabled(false);
  return DeviceBase::updatePlayerState(cmd);
}

void HesaiFT120::openCorrectionFileClicked() {
  // choose pcap with dialog
  QString pcap_file = GetOpenFileName("Open Pcap File", last_pcap_open_dirpath_,
                                      "All Files(*.*)");
  if (pcap_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    label_correction_file_->setText("no pcap file");
    return;
  }
  correction_file_path_ = pcap_file.toStdString();
  label_correction_file_->setText(pcap_file);
}

bool HesaiFT120::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  std::string str;
  config->getParameter("port", str);
  edit_udp_port_->setText(QString::fromStdString(str));
  config->getParameter("ip", str);
  edit_udp_ip_->setText(QString::fromStdString(str));
  config->getParameter("pcap", crt_pcap_path_);
  label_pcap_file_->setText(QString::fromStdString(crt_pcap_path_));
  config->getParameter("correction", correction_file_path_);
  label_correction_file_->setText(
      QString::fromStdString(correction_file_path_));
  manipulator_->InitFromConfig(config);
  return DeviceBase::initFromConfig(config);
}

bool HesaiFT120::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter("port", edit_udp_port_->text().toStdString());
  config->setParameter("ip", edit_udp_ip_->text().toStdString());
  config->setParameter("pcap", crt_pcap_path_);
  config->setParameter("correction", correction_file_path_);
  manipulator_->StoreToConfig(config);
  return DeviceBase::storeToConfig(config);
}
