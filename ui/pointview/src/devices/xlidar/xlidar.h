#ifndef XLIDAR_H
#define XLIDAR_H

#include <QCheckBox>

#include "utils/adc_plot.h"
#include "utils/camera/video_capture.h"
#include "utils/camera/video_player.h"
#include "utils/common/lidar_base.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/io/simple_player_setting.h"
#include "utils/io/udp_input.h"
#include "utils/lidar/laser_track.h"
#include "utils/lidar/lidar_frame_info.h"
#include "utils/lidar/lidar_intrinsics.h"
#include "utils/lidar/lidar_return_info.h"
#include "utils/lidar/range_image.h"
#include "utils/pointcloud/point_data.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_filter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"
#include "utils/udp_logger.h"

namespace autox {
namespace drivers {
namespace xlidar {
struct PointCloud;
class XLidarDriver;

using RawPointCloud = autox::drivers::xlidar::PointCloud;

class XLidar : public autox::pointview::LidarBase<RawPointCloud> {
  Q_OBJECT

 public:
  struct Frame {
    std::shared_ptr<RawPointCloud> raw_pointcloud;
    PointCloudT::Ptr pcl_pointcloud;
    QImage range_image;
    //
    vtkSmartPointer<vtkUnsignedCharArray> color_scalars;
    size_t n_valid_points;
    // from raw_pointcloud
    double timestamp;
    size_t n_udp_packets;
    size_t n_points;
    uint8_t version;
    uint8_t max_return_num;
  };
  XLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
         int device_id, const std::string& device_name);
  ~XLidar();
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
  void resetFrame(Frame& frame);
  void convertToPclPointCloud(Frame& frame);
  void convertToRangeImage(Frame& frame);
  void processPclPoint(Frame& frame);
  bool update();

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // helper utils
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  std::shared_ptr<autox::pointview::LaserTrack> laser_track_;
  std::shared_ptr<autox::pointview::RangeImage> range_image_;
  std::shared_ptr<autox::pointview::VideoPlayer> video_player_;
  std::shared_ptr<autox::pointview::PointFilter> point_filter_;
  std::shared_ptr<autox::pointview::LidarIntrinsics> lidar_intrinsics_;
  std::shared_ptr<autox::pointview::UdpLogger> udp_logger_;
  std::shared_ptr<autox::pointview::PointData> point_data_;
  // ui for debug
  std::shared_ptr<QCheckBox> checkbox_range_image_correction_;
  bool enable_range_image_correction_{false};
  std::shared_ptr<QCheckBox> checkbox_fast_render_;
  bool enable_fast_render_{false};
  // table heads
  QStringList table_head_{
      "laser id",    "return id", "encoder",    "elevation",   "azimuth",
      "distance",    "intensity", "elongation", "timestamp",   "x",
      "y",           "z",         "section",    "adc_channel", "distance_group",
      "azimuth_code"};
  // driver
  std::shared_ptr<autox::drivers::xlidar::XLidarDriver> driver256_;
  // point cloud
  Frame current_frame_;
  // frame for rendering
  std::deque<Frame> rendering_frame_buffer_;
  std::deque<Frame> unused_frame_buffer_;
  size_t max_frame_buffer_size_{2};
  std::mutex frame_mutex_;
  // point cloud buffer
  std::deque<std::shared_ptr<RawPointCloud>> raw_pointcloud_buffer_;
  std::mutex raw_pointcloud_mutex_;
  // for multi-thread
  size_t num_threads_{2};
  int driver_thread_num_{0};
  // lidar setting
  float min_distance_{0.1};
  float max_distance_{262};
  float hfov_start_{0};
  float hfov_end_{360};
  // data
  int laser_num_{256};
  double azimuth_resolution_{0.012};
  std::vector<int> laser_idx_map_;
  // thread
  std::unique_ptr<std::thread> data_thread_;
  bool exit_{false};
  // adc
  std::shared_ptr<autox::pointview::AdcPlot> adc_plot_;
  long long record_frame_index{0};
  // video capture
  std::shared_ptr<autox::pointview::VideoCapture> video_capture_;
};

}  // namespace xlidar
}  // namespace drivers
}  // namespace autox
#endif  // XLIDAR_H
