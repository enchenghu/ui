#ifndef XRADAR_2D_H
#define XRADAR_2D_H

#include <QCheckBox>

#include "utils/common/lidar_base.h"
#include "utils/pointcloud/point_data.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_filter.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/radar/rf_image.h"

namespace autox {
namespace drivers {
namespace xradar_2d {
struct PointCloud;
class XRadar2DDriver;

using RawPointCloud = autox::drivers::xradar_2d::PointCloud;

class XRadar2D : public autox::pointview::LidarBase<RawPointCloud> {
  Q_OBJECT

 public:
  struct Frame {
    std::shared_ptr<RawPointCloud> raw_pointcloud;
    PointCloudT::Ptr pcl_pointcloud;
    size_t n_valid_points;
    // from raw_pointcloud
    double timestamp;
    size_t n_udp_packets;
    size_t n_points;
    uint8_t major_version;
    uint8_t minor_version;
  };
  XRadar2D(std::shared_ptr<autox::pointview::DisplayContext> context,
           autox::pointview::DeviceBaseParameter& parameter);
  ~XRadar2D();
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
  void resetFrame(Frame& frame);
  void convertToPclPointCloud(Frame& frame);

  autox::pointview::rf_image::PointCloud GenerateRfPointcloud();

 private:
  static constexpr int kXRadar2DAzimuthBins = 160;
  static constexpr int kXRadar2DRangeBins = 256;
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // helper utils
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  std::shared_ptr<autox::pointview::PointFilter> point_filter_;
  std::shared_ptr<autox::pointview::PointData> point_data_;
  std::shared_ptr<autox::pointview::rf_image::RfImage> rf_viewer_;
  // table heads
  QStringList table_head_{"range_code",
                          "doppler_code",
                          "azimuth_code",
                          "log_power",
                          "timestamp",
                          "distance",
                          "detect_type",
                          "x",
                          "y",
                          "z",
                          "azimuth",
                          "doppler"};
  // driver
  std::shared_ptr<autox::drivers::xradar_2d::XRadar2DDriver> driver_;
  // point cloud
  Frame current_frame_;
  // point cloud buffer
  std::deque<std::shared_ptr<RawPointCloud>> raw_pointcloud_buffer_;
  std::mutex raw_pointcloud_mutex_;
  // lidar setting
  float min_distance_{0.1};
  float max_distance_{262};
  float hfov_start_{0};
  float hfov_end_{360};
};

}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox
#endif  // XRADAR_2D_H
