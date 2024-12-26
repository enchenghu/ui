#ifndef HESAI_QT64_H
#define HESAI_QT64_H

#include "utils/common/device_base.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/io/simple_player_setting.h"
#include "utils/io/udp_input.h"
#include "utils/lidar/laser_track.h"
#include "utils/lidar/lidar_frame_info.h"
#include "utils/lidar/lidar_intrinsics.h"
#include "utils/lidar/lidar_return_info.h"
#include "utils/lidar/range_image.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_filter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"

namespace autox {
namespace drivers {
namespace hesai_qt64 {
struct PointCloud;
class DriverQt64;
}  // namespace hesai_qt64
}  // namespace drivers
}  // namespace autox

class HesaiQt64 : public autox::pointview::DeviceBase {
  Q_OBJECT

 public:
  using RawPointCloud = autox::drivers::hesai_qt64::PointCloud;
  using PlaybackBuffer = autox::pointview::PlaybackBuffer<RawPointCloud>;
  struct Frame {
    std::shared_ptr<RawPointCloud> raw_pointcloud;
    PointCloudT::Ptr pcl_pointcloud;
    // info
    double timestamp;
    size_t n_udp_packets;
    size_t n_points;
    size_t n_valid_points;
  };
  HesaiQt64(std::shared_ptr<autox::pointview::DisplayContext> context,
            autox::pointview::DeviceBaseParameter& parameter);
  ~HesaiQt64();
  bool updateUI() override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;
  bool updatePlayerState(autox::pointview::PlayerCmd cmd) override;

 protected:
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
  std::shared_ptr<autox::pointview::LidarFrameInfo> info_;
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  std::shared_ptr<autox::pointview::SimplePlayerSetting> player_setting_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::LaserTrack> laser_track_;
  std::shared_ptr<autox::pointview::LidarReturnInfo> return_mode_info_;
  std::shared_ptr<autox::pointview::RangeImage> range_image_;
  std::shared_ptr<autox::pointview::PointFilter> point_filter_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  // table heads
  QStringList table_head_{"laser_id", "encoder",   "elevation",   "azimuth",
                          "distance", "intensity", "time offset", "x",
                          "y",        "z"};
  // driver
  std::shared_ptr<autox::drivers::hesai_qt64::DriverQt64> driver_;
  // for live streaming and playback
  std::shared_ptr<autox::pointview::UdpInput> udp_input_;
  std::shared_ptr<PlaybackBuffer> playback_buffer_;
  std::shared_ptr<autox::pointview::PcapUdpParser> pcap_parser_;
  // raw pointcloud buffer
  std::deque<std::shared_ptr<RawPointCloud>> raw_pointcloud_buffer_;
  std::mutex raw_pointcloud_buffer_mutex_;
  // current frame
  Frame current_frame_;
};

#endif  // HESAI_QT64_H
