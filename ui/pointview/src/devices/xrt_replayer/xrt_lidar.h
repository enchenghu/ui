/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_LIDAR_H
#define XRT_LIDAR_H

#include <QLineEdit>
#include <QPushButton>
#include <thread>

#include "utils/common/device_base.h"
#include "utils/frame_info.h"
#include "utils/pointcloud/point_exporter.h"
#include "utils/pointcloud/point_manipulator.h"
#include "utils/pointcloud/point_selection.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/pose_setting.h"

#include "xrt_message_dispatcher.h"

namespace autox {
namespace drivers {
class PointCloudPack;
}  // namespace drivers
}  // namespace autox

class XrtLidar : public autox::pointview::DeviceBase {
  Q_OBJECT

 public:
  using RawPointCloud = autox::drivers::PointCloudPack;
  XrtLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
           int device_id, const std::string& device_name);
  ~XrtLidar();
  bool updateUI() override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 private:
  bool getPointInfo(size_t idx, std::vector<double>& data);
  size_t convertToPclPointCloud(int channel_idx);
  bool pausePlayer();

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // Ui object
  std::shared_ptr<QLineEdit> edit_channel_name_;
  std::shared_ptr<QPushButton> btn_select_channel_;
  // helper utils
  std::shared_ptr<autox::pointview::FrameInfo> info_;
  std::shared_ptr<autox::pointview::PoseSetting> pose_setting_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  std::shared_ptr<autox::pointview::PointSelection> point_selection_;
  std::shared_ptr<autox::pointview::PointExporter> point_exporter_;
  // table heads
  QStringList table_head_{"laser id",  "x",          "y",
                          "z",         "distance",   "intensity",
                          "timestamp", "confidence", "doppler"};
  // point cloud
  std::mutex lock;
  PointCloudT::Ptr cloud_;
  std::shared_ptr<RawPointCloud> raw_point_cloud_;
  // xrt channel dispatcher
  XrtMessageDispatcher* dispatcher_;
  std::string channel_name_{"/xurban/sensor/lidar/PointCloudPack"};
  // thread
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
  // swap buffer
  std::shared_ptr<RawPointCloud> swap_point_cloud_{nullptr};
  bool buffer_full_{false};
};

#endif  // XRT_LIDAR_H
