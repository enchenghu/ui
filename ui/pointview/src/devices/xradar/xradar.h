#ifndef XRADAR_H
#define XRADAR_H

#include "utils/common/device_base.h"
#include "utils/frame_info.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/io/simple_player_setting.h"
#include "utils/io/udp_input.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"
#include "xradar_setting.h"

namespace autox {
namespace drivers {
namespace arbe {
class ArbeDriver;
class RawPointCloud;
}  // namespace arbe
}  // namespace drivers
}  // namespace autox

class XRadar : public autox::pointview::DeviceBase {
  Q_OBJECT
 public:
  using RawPointCloud = autox::drivers::arbe::RawPointCloud;
  using PlaybackBuffer = autox::pointview::PlaybackBuffer<RawPointCloud>;
  XRadar(std::shared_ptr<autox::pointview::DisplayContext> context,
         autox::pointview::DeviceBaseParameter& parameter);
  ~XRadar() override;
  bool updateUI() override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;
  bool updatePlayerState(autox::pointview::PlayerCmd cmd) override;

 private:
  void initDriver();
  size_t convertToPclPointCloud(size_t start, size_t end, int channel_idx);
  bool getPointInfo(size_t idx, std::vector<double>& data);

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // helper utils
  std::shared_ptr<autox::pointview::FrameInfo> info_;
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  std::shared_ptr<autox::pointview::SimplePlayerSetting> player_setting_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  std::shared_ptr<autox::pointview::XRadarSetting> xradar_setting_;

  // point cloud data and swap buffer
  PointCloudT::Ptr cloud_;
  std::shared_ptr<RawPointCloud> raw_point_cloud_;
  std::shared_ptr<RawPointCloud> swap_point_cloud_;
  bool buffer_full_{false};
  std::mutex lock;
  // driver
  std::shared_ptr<autox::drivers::arbe::ArbeDriver> driver_;
  // for live streaming
  std::shared_ptr<autox::pointview::UdpInput> udp_input_;
  // for pcap playback
  std::shared_ptr<autox::pointview::PcapUdpParser> pcap_parser_;
  std::shared_ptr<PlaybackBuffer> playback_buffer_;
  // table heads
  QStringList table_head_{"elevation",   "azimuth", "range", "doppler", "power",
                          "time offset", "x",       "y",     "z"};
};

#endif  // XRADAR_H
