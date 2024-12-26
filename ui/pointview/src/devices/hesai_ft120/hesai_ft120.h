#ifndef HESAI_FT120_H
#define HESAI_FT120_H

#include <pcl/point_types.h>

#include "utils/common/device_base.h"
#include "utils/lidar/lidar_frame_info.h"
#include "utils/lidar/range_image.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_filter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"

struct PointXYZIT {
  PCL_ADD_POINT4D
  float intensity;
  double timestamp;
  uint16_t ring;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT(
    PointXYZIT,
    (float, x, x)(float, y, y)(float, z, z)(float, intensity, intensity)(
        double, timestamp, timestamp)(uint16_t, ring, ring))
namespace hesai {
namespace lidar {
template <typename T_Point>
class HesaiLidarSdk;
}
}  // namespace hesai

class HesaiFT120 : public autox::pointview::DeviceBase {
  Q_OBJECT

 public:
  using RawPointCloud = pcl::PointCloud<PointXYZIT>;
  struct Frame {
    std::shared_ptr<RawPointCloud> raw_pointcloud;
    PointCloudT::Ptr pcl_pointcloud;
    // info
    double timestamp;
    size_t n_udp_packets;
    size_t n_points;
    size_t n_valid_points;
  };
  HesaiFT120(std::shared_ptr<autox::pointview::DisplayContext> context,
             autox::pointview::DeviceBaseParameter& parameter);
  ~HesaiFT120();
  bool updateUI() override;
  bool updatePlayerState(autox::pointview::PlayerCmd cmd) override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 protected:
  void initDriver();
  bool getPointInfo(size_t idx, std::vector<double>& data);
  bool getPointUID(size_t idx, int& uid);
  void convertToPclPointCloud(Frame& frame);

 private slots:
  void playerTypeChanged(int index);
  bool openPcapClicked();
  void openCorrectionFileClicked();

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // helper utils
  std::shared_ptr<autox::pointview::LidarFrameInfo> info_;
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  // table heads
  QStringList table_head_{"x", "y", "z"};

  // raw pointcloud buffer
  std::deque<std::shared_ptr<RawPointCloud>> raw_pointcloud_buffer_;
  std::mutex raw_pointcloud_buffer_mutex_;
  // current frame
  Frame current_frame_;

  std::shared_ptr<hesai::lidar::HesaiLidarSdk<PointXYZIT>> hesai_sdk_;
  std::shared_ptr<QComboBox> combobox_player_type_;
  std::shared_ptr<QLineEdit> edit_udp_port_;
  std::shared_ptr<QLineEdit> edit_udp_ip_;
  std::shared_ptr<QPushButton> btn_pcap_file_;
  std::shared_ptr<QLabel> label_pcap_file_;
  QString last_pcap_open_dirpath_;
  std::string crt_pcap_path_;
  std::shared_ptr<QPushButton> correction_file_button_;
  std::string correction_file_path_;
  std::shared_ptr<QLabel> label_correction_file_;
};

#endif  // HESAI_FT120_H
