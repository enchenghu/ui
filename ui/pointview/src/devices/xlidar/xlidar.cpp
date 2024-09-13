
#include "xlidar.h"

#include <omp.h>

#include <QMessageBox>
#include <iostream>

#include "driver/xlidar_driver.h"

#define MAX_RECORDED_UDP_PACKETS (20000 * 10 * 30)  // 30s

using namespace std::chrono_literals;

namespace autox {
namespace drivers {
namespace xlidar {
XLidar::XLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
               int device_id, const std::string& device_name)
    : LidarBase<RawPointCloud>(context, device_id, device_name),
      viewer_(context->getViewerPtr()) {
  laser_idx_map_.resize(laser_num_);
  for (int i = 0; i < laser_num_; i++) {
    laser_idx_map_[i] = i;
  }
  // point cloud manipulator
  channel_settings_.push_back({"intensity", "jet", 0, 255});
  channel_settings_.push_back({"elongation", "jet", 0, 255});
  channel_settings_.push_back({"laser block", "rainbow", 1, 8});
  channel_settings_.push_back({"return id", "rainbow", 1, 4});
  channel_settings_.push_back({"laser id", "AutoxMaxRange", 0, 255});
  // lidar base init
  InitLidarBase();
  initDriver();
  // add point cloud to pcl viewer
  resetFrame(current_frame_);
  viewer_->addPointCloud(current_frame_.pcl_pointcloud,
                         std::to_string(device_context_->getDeviceId()));
  // add plugin
  // adc plot
  adc_plot_ = std::make_shared<autox::pointview::AdcPlot>(device_context_);
  // laser_track
  laser_track_ = std::make_shared<autox::pointview::LaserTrack>(device_context_,
                                                                table_head_);
  laser_track_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
      });
  laser_track_->setPointCheckCallback([this](size_t i) {
    auto& p = current_frame_.raw_pointcloud->points[i];
    return laser_track_->checkPoint(p.laser_id + 1, p.encoder);
  });
  // point_data
  point_data_ = std::make_shared<autox::pointview::PointData>(device_context_,
                                                              table_head_);
  point_data_->setPointInfoCallback(
      [this](size_t idx, std::vector<double>& data) {
        return getPointInfo(idx, data);
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
  // range image
  range_image_ = std::make_shared<autox::pointview::RangeImage>(
      device_context_, 10000, laser_num_);
  // video capture
  video_capture_ =
      std::make_shared<autox::pointview::VideoCapture>(device_context_);
  // video player
  video_player_ =
      std::make_shared<autox::pointview::VideoPlayer>(device_context_);
  // point cloud pose
  pose_setting_ =
      std::make_shared<autox::pointview::PoseSetting>(device_context_);

  // player setting
  player_setting_->setSyncPlayerCb([this]() { emit SyncPlayer(); });
  player_setting_->setResetDriverCb([this](bool is_playback) {
    if (is_playback) {
      player_setting_->setDriverThreadNum(8);
    } else {
      player_setting_->setDriverThreadNum(0);
    }
    driver256_->ResetDriver();
  });
  player_setting_->setDriverThreadCb([this](int num) {
    driver256_->SetMultiThreadMode(num);
    driver256_->ResetDriver();
  });
  player_setting_->setUdpPort("2370");
  // logger
  udp_logger_ = std::make_unique<autox::pointview::UdpLogger>(device_context_);
  // lidar correction
  lidar_intrinsics_ = std::make_shared<autox::pointview::LidarIntrinsics>(
      device_context_, laser_num_);
  lidar_intrinsics_->registerSetIntrinsicsCallback(
      [this](const std::vector<double>& elevation_offset,
             const std::vector<double>& azimuth_offset,
             const std::vector<double>& distance_offset,
             const std::vector<double>& origin_x_offset,
             const std::vector<double>& origin_y_offset,
             const std::vector<double>& origin_z_offset) {
        driver256_->SetIntrinsics(elevation_offset, azimuth_offset,
                                  distance_offset, origin_x_offset,
                                  origin_y_offset, origin_z_offset);
        // update laser_idx_map_
        std::vector<int> index;
        for (size_t i = 0; i < elevation_offset.size(); i++) {
          index.push_back(i);
        }
        std::sort(index.begin(), index.end(), [&](const int& a, const int& b) {
          return (elevation_offset[a] > elevation_offset[b]);
        });
        if (index.size() == laser_idx_map_.size()) {
          for (size_t i = 0; i < index.size(); i++) {
            laser_idx_map_[index[i]] = i;
          }
        }
        // reload pcap if playback
        autox::pointview::PlayerCmd cmd;
        cmd.type = autox::pointview::PlayerCmdType::ReloadPlayback;
        device_context_->updatePlayerState(cmd);
        return true;
      });
  lidar_intrinsics_->registerGetIntrinsicsCallback(
      [this](std::vector<double>& elevation_offset,
             std::vector<double>& azimuth_offset,
             std::vector<double>& distance_offset,
             std::vector<double>& origin_x_offset,
             std::vector<double>& origin_y_offset,
             std::vector<double>& origin_z_offset) {
        return driver256_->GetIntrinsics(elevation_offset, azimuth_offset,
                                         distance_offset, origin_x_offset,
                                         origin_y_offset, origin_z_offset);
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
  debug_sub->addProperty("RI correction", checkbox_range_image_correction_);
  checkbox_fast_render_ = std::make_shared<QCheckBox>();
  debug_sub->addProperty("fast render", checkbox_fast_render_);
  // signal and slots
  connect(checkbox_range_image_correction_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_range_image_correction_ = state > 0;
            device_context_->refreshPointCloud();
          });
  connect(checkbox_fast_render_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_fast_render_ = state > 0;
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
  //
  data_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      if (!update()) {
        std::this_thread::sleep_for(10ms);
      }
    }
  });
  pthread_setname_np(data_thread_->native_handle(), "pv/data");
  //
  omp_set_num_threads(num_threads_);
  std::cout << "OpenMP will use threads:" << omp_get_max_threads() << std::endl;
}

XLidar::~XLidar() {
  exit_ = true;
  if (data_thread_ && data_thread_->joinable()) {
    data_thread_->join();
  }
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

void XLidar::initDriver() {
  // callback
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    driver256_->ParseLidarPacket(data, len);
  };

  auto set_packet_index_cb = [this](size_t index) {
    driver256_->SetCurPacketIndex(index);
  };

  auto set_packet_offset_cb = [this](long int offset) {
    driver256_->SetCurPacketOffset(offset);
  };

  auto reset_cb = [this](void) { driver256_->ResetDriver(); };

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
      if (device_context_->getPlayerState().is_recording) {
        device_context_->updateCurrentFrame(record_frame_index++);
      } else {
        record_frame_index = 0;
      }
      if (raw_pointcloud_buffer_.size() > 2) {
        raw_pointcloud_buffer_.pop_front();
        std::cout << "[warning] converting too long, drop raw point cloud!"
                  << std::endl;
      }
    }
  };
  // create driver
  driver256_ = std::make_shared<autox::drivers::xlidar::XLidarDriver>();
  driver256_->SetPointCloudCallback(point_cloud_cb);
  driver256_->SetMultiThreadMode(driver_thread_num_);
  // for live streaming
  // 16M: 16*1024*1024 = 16777216
  udp_input_->setSocketRecvBufferSize(16777216);
  udp_input_->setMaxRecordPacket(MAX_RECORDED_UDP_PACKETS);
  udp_input_->setUdpCallback(udp_cb);
  udp_input_->setMaxPacketQueueSize(100000);
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

void XLidar::resetFrame(Frame& frame) {
  frame.pcl_pointcloud.reset(new PointCloudT);
}

void XLidar::convertToPclPointCloud(Frame& frame) {
  int channel_idx = manipulator_->getChannelIndex();
  auto colormap = manipulator_->getColormap();
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  size_t num_points = frame.raw_pointcloud->points.size();
// use OpenMp to accelerate point convertion
#pragma omp parallel for num_threads(num_threads_)
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
      v = p.intensity;
    } else if (channel_idx == 6) {
      // elongation
      v = p.elongation;
    } else if (channel_idx == 7) {
      // laser id
      v = 1 + p.laser_id % 8;
    } else if (channel_idx == 8) {
      // return id
      v = p.return_id + 1;
    } else if (channel_idx == 9) {
      v = p.laser_id;
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

void XLidar::processPclPoint(Frame& frame) {
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& color_scalars = frame.color_scalars;
  size_t num_points = pcl_pointcloud->points.size();
  // init color scalars
  color_scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
  color_scalars->SetNumberOfComponents(4);
  vtkIdType nr_points = num_points;
  color_scalars->SetNumberOfTuples(nr_points);

  unsigned char* point_cloud_color = color_scalars->GetPointer(0);
  size_t cnt = 0;
  for (size_t i = 0; i < num_points; i++) {
    auto& p = pcl_pointcloud->points[i];
    // color scalars
    int idx = static_cast<int>(i) * 4;
    point_cloud_color[idx] = p.r;
    point_cloud_color[idx + 1] = p.g;
    point_cloud_color[idx + 2] = p.b;
    point_cloud_color[idx + 3] = p.a;
    if (p.a != 0) {
      cnt++;
    }
  }

  frame.n_valid_points = cnt;
}

void XLidar::convertToRangeImage(Frame& frame) {
  auto& pcl_pointcloud = frame.pcl_pointcloud;
  auto& raw_pointcloud = frame.raw_pointcloud;
  auto& image = frame.range_image;
  size_t num_points = raw_pointcloud->points.size();
  // init range image
  image = QImage(10000, laser_num_, QImage::Format_RGB32);
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
        range_image_x = (int)(atan2(p.x, p.y) * 5000 / M_PI);
        if (range_image_x < 0) {
          range_image_x += 10000;
        }
        range_image_x = std::clamp(range_image_x, 0, 9999);
      } else {
        range_image_x = (int)(p.azimuth / 0.036);
      }
      int range_image_y = laser_idx_map_[p.laser_id];
      auto range_image_color = qRgb(uint(p2.r), uint(p2.g), uint(p2.b));
      QRgb* row = image_row_ptr[range_image_y];
      row[range_image_x] = range_image_color;
      adc_plot_->set_azimuth_code(range_image_x, range_image_y, p.azimuth_code);
    }
  }
}

bool XLidar::update() {
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
  auto start = std::chrono::steady_clock::now();
  auto& raw_pointcloud = current_frame_.raw_pointcloud;
  auto& pcl_pointcloud = current_frame_.pcl_pointcloud;
  // get info from raw_pointcloud
  current_frame_.timestamp = raw_pointcloud->timestamp;
  current_frame_.n_udp_packets = raw_pointcloud->udp_packet_number;
  current_frame_.n_points = raw_pointcloud->points.size();
  current_frame_.version = raw_pointcloud->version;
  current_frame_.max_return_num = raw_pointcloud->max_return_num;
  // convert raw_pointcloud to pcl_pointcloud
  pcl_pointcloud->resize(current_frame_.n_points);
  auto end1 = std::chrono::steady_clock::now();
  convertToPclPointCloud(current_frame_);
  auto end2 = std::chrono::steady_clock::now();
  // transform
  pose_setting_->transform(pcl_pointcloud);
  // update point_filter_
  point_filter_->update(pcl_pointcloud);
  // mark for point_selection
  point_selection_->update(pcl_pointcloud, false);
  // point data
  point_data_->update(pcl_pointcloud);
  // for intrinsics calibration
  lidar_intrinsics_->update(pcl_pointcloud);
  // update pcl color and get valid point count
  auto end3 = std::chrono::steady_clock::now();
  processPclPoint(current_frame_);
  auto end4 = std::chrono::steady_clock::now();
  // update range image
  convertToRangeImage(current_frame_);
  auto end5 = std::chrono::steady_clock::now();
  // export
  point_exporter_->update(pcl_pointcloud);
  // update flag
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  // move current_frame_ to rendering_frame_buffer_
  {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    rendering_frame_buffer_.push_back(current_frame_);
    if (rendering_frame_buffer_.size() > max_frame_buffer_size_) {
      unused_frame_buffer_.push_back(rendering_frame_buffer_.front());
      rendering_frame_buffer_.pop_front();
      std::cout << "[warning] rendering too long, drop point cloud frame!"
                << std::endl;
    }
    std::shared_ptr<RawPointCloud> current_raw_pointcloud = raw_pointcloud;
    if (unused_frame_buffer_.size() > 0) {
      current_frame_ = unused_frame_buffer_.front();
      unused_frame_buffer_.pop_front();
    } else {
      resetFrame(current_frame_);
    }
    // keep current raw pointcloud
    current_frame_.raw_pointcloud = current_raw_pointcloud;
  }
  // debug info
  auto converting_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  auto pcl_pointcloud_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end2 - end1)
          .count();
  auto color_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end4 - end3)
          .count();
  auto range_image_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end5 - end4)
          .count();
  std::cout << "udp packet number: " << current_frame_.n_udp_packets
            << ", total point number: " << current_frame_.n_points
            << ", valid point number: " << current_frame_.n_valid_points
            << std::endl
            << "converting time: " << converting_time
            << ", to pcl: " << pcl_pointcloud_time
            << ", set pcl color: " << color_time
            << ", to range_image: " << range_image_time << std::endl;
  return true;
}

bool XLidar::updateUI() {
  // update udp speed
  player_setting_->refreshUdpSpeed();
  adc_plot_->Update();
  Frame frame;
  {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    if (rendering_frame_buffer_.empty()) {
      return false;
    }
    frame = rendering_frame_buffer_.front();
    rendering_frame_buffer_.pop_front();
  }
  auto start = std::chrono::steady_clock::now();
  // update point selection
  point_selection_->refreshTable();
  // update udp speed
  player_setting_->refreshUdpSpeed();
  // update laser track
  // TODO (all, need mark point cloud in update())
  laser_track_->update(frame.pcl_pointcloud);
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  // update point cloud
  // TODO(all) updatePointCloud will cause crash when the count of pointcloud
  // changed a lot, refer to https://github.com/PointCloudLibrary/pcl/pull/4017
  // (it's solved in pcl version1.11). another solution is using remove & add
  // instead of update pointcloud, it works well but not elegant. by the way,
  // for performace, remove & add API is faster than update API, which is
  // intresting.
  // update Udp Index in Pcap
  if (device_context_->getPlayerState().is_playback)
    player_setting_->updateUdpIndexPcap(
        frame.raw_pointcloud->begin_index_in_pacp,
        frame.raw_pointcloud->end_index_in_pacp);
  if (enable_fast_render_) {
    viewer_->customupdatePointCloud(frame.pcl_pointcloud, frame.color_scalars,
                                    cloud_id);
  } else {
    viewer_->removePointCloud(cloud_id);
    viewer_->customAddPointCloud(frame.pcl_pointcloud, frame.color_scalars,
                                 cloud_id);
  }
  // update point hide/size
  manipulator_->update(cloud_id);
  auto end1 = std::chrono::steady_clock::now();
  // update range image
  range_image_->Update(frame.range_image);
  // set info
  info_->update(frame.timestamp, frame.n_valid_points, frame.max_return_num,
                frame.n_udp_packets, 4, frame.version);
  // move frame to unused frame buffer
  {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    unused_frame_buffer_.push_back(frame);
  }
  auto end = std::chrono::steady_clock::now();
  // debug info
  auto rendering_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  auto pointcloud_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start)
          .count();
  std::cout << "rendering time: " << rendering_time
            << ", pointcloud: " << pointcloud_time << std::endl;
  return true;
}

bool XLidar::getPointInfo(size_t idx, std::vector<double>& data) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  data = std::vector<double>(
      {(double)p.laser_id + 1, (double)p.return_id + 1, (double)p.encoder,
       p.elevation, p.azimuth, p.distance, (double)p.intensity,
       (double)p.elongation, p.timestamp, p.x, p.y, p.z, (double)p.section,
       (double)adc_plot_->adc_channel(p.laser_id),
       (double)adc_plot_->distance_group(p.laser_id), (double)p.azimuth_code});
  return true;
}

bool XLidar::getPointUID(size_t idx, int& uid) {
  if (idx >= current_frame_.raw_pointcloud->points.size()) {
    return false;
  }
  auto& p = current_frame_.raw_pointcloud->points[idx];
  uid = p.encoder / azimuth_resolution_ * 10000 + p.laser_id * 10 + p.return_id;
  return true;
}

bool XLidar::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  // init driver
  config->getParameter("driver.thread_num", driver_thread_num_);
  if (driver_thread_num_ > 0) {
    driver256_->SetMultiThreadMode(driver_thread_num_);
  }

  config->getParameter("driver.min_distance", min_distance_);
  config->getParameter("driver.max_distance", max_distance_);
  config->getParameter("driver.hfov_start", hfov_start_);
  config->getParameter("driver.hfov_end", hfov_end_);

  driver256_->config(min_distance_, max_distance_, hfov_start_, hfov_end_);
  //
  pose_setting_->initFromConfig(config);

  laser_track_->initFromConfig(config);
  range_image_->InitFromConfig(config);
  point_filter_->initFromConfig(config);
  lidar_intrinsics_->initFromConfig(config);

  adc_plot_->InitFromConfig(config);
  return LidarBase::initFromConfig(config);
}

bool XLidar::storeToConfig(std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter("driver.thread_num", driver_thread_num_);
  config->setParameter("driver.min_distance", min_distance_);
  config->setParameter("driver.max_distance", max_distance_);
  config->setParameter("driver.hfov_start", hfov_start_);
  config->setParameter("driver.hfov_end", hfov_end_);
  pose_setting_->storeToConfig(config);

  laser_track_->storeToConfig(config);
  range_image_->StoreToConfig(config);
  point_filter_->storeToConfig(config);
  lidar_intrinsics_->storeToConfig(config);

  adc_plot_->StoreToConfig(config);
  return LidarBase::storeToConfig(config);
}

void XLidar::initPointFilterSetting(
    autox::pointview::PointFilterSetting& point_filter_setting) {
  point_filter_setting.addIntItem(
      "laser id", "", 1, laser_num_,
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
      "return id", "", 1, 8, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.return_id + 1;
        return true;
      });
  point_filter_setting.addDoubleItem(
      "encoder(360deg)", "deg", 0, 360, [this](size_t idx, double& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.encoder;
        return true;
      });
  point_filter_setting.addIntItem(
      "encoder(10000)", "", 0, 10000, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.encoder / 0.036;
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
  point_filter_setting.addIntItem(
      "elongation", "", 0, 255, [this](size_t idx, int& data) {
        if (idx >= current_frame_.raw_pointcloud->points.size()) {
          return false;
        }
        auto& p = current_frame_.raw_pointcloud->points[idx];
        data = p.elongation;
        return true;
      });
}

bool XLidar::updatePlayerState(autox::pointview::PlayerCmd cmd) {
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
}  // namespace xlidar
}  // namespace drivers
}  // namespace autox