/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef BLIDAR_H
#define BLIDAR_H

#include "utils/camera/video_player.h"
#include "utils/common/lidar_base.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/io/simple_player_setting.h"
#include "utils/io/udp_input.h"
#include "utils/lidar/lidar_frame_info.h"
#include "utils/lidar/lidar_intrinsics.h"
#include "utils/lidar/range_image.h"
#include "utils/pointcloud/point_data.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_filter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"

namespace autox {
namespace drivers {
namespace blidar {
struct PointCloud;
class Driver;
using RawPointCloud = autox::drivers::blidar::PointCloud;

class BLidar : public autox::pointview::LidarBase<RawPointCloud> {
  Q_OBJECT

 public:
  struct Frame {
    std::shared_ptr<RawPointCloud> raw_pointcloud;
    PointCloudT::Ptr pcl_pointcloud;
    // info
    double timestamp;
    size_t n_udp_packets;
    size_t n_points;
    size_t n_valid_points;
    uint8_t version;
    uint8_t max_return_num;
  };
  BLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
         int device_id, const std::string& device_name);
  ~BLidar();
  bool updateUI() override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;
  bool updatePlayerState(autox::pointview::PlayerCmd cmd) override;

 private:
  void initDriver();
  void initPointFilterSetting(
      autox::pointview::PointFilterSetting& point_filter_setting);
  bool getPointInfo(size_t idx, std::vector<double>& data);
  bool getPointUID(size_t idx, int& uid);
  void convertToPclPointCloud(Frame& frame);

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // helper utils
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  std::shared_ptr<autox::pointview::RangeImage> range_image_;
  std::shared_ptr<autox::pointview::PointFilter> point_filter_;
  std::shared_ptr<autox::pointview::LidarIntrinsics> lidar_intrinsics_;
  std::shared_ptr<autox::pointview::PointData> point_data_;
  std::shared_ptr<autox::pointview::VideoPlayer> video_player_;
  // table heads
  QStringList table_head_{"laser id",
                          "return id",
                          "frame id",
                          "elevation encoder",
                          "azimuth encoder",
                          "elevation",
                          "azimuth",
                          "distance",
                          "intensity",
                          "elongation",
                          "timestamp",
                          "x",
                          "y",
                          "z"};
  // driver
  std::shared_ptr<autox::drivers::blidar::Driver> driver_;
  // raw pointcloud buffer
  std::deque<std::shared_ptr<RawPointCloud>> raw_pointcloud_buffer_;
  std::mutex raw_pointcloud_buffer_mutex_;
  // current frame
  Frame current_frame_;
  long long record_frame_index{0};
};

}  // namespace blidar
}  // namespace drivers
}  // namespace autox

#endif  // BLIDAR_H
