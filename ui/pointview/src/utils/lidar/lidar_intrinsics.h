/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTCLOUD_LIDAR_INTRINSICS_H
#define POINTCLOUD_LIDAR_INTRINSICS_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/pointcloud/pointcloud.h"

namespace autox {
namespace pointview {

class LidarIntrinsics : public QObject {
  Q_OBJECT

 public:
  // Elevation,Azimuth,Distance,Origin X,Origin Y,Origin Z
  using SetIntrinsicsCallback = std::function<bool(
      const std::vector<double>& elevation, const std::vector<double>& azimuth,
      const std::vector<double>& distance)>;
  using GetIntrinsicsCallback = std::function<bool(
      std::vector<double>& elevation, std::vector<double>& azimuth,
      std::vector<double>& distance)>;
  using SetIntrinsicsCallback2 = std::function<bool(
      const std::vector<double>& elevation, const std::vector<double>& azimuth,
      const std::vector<double>& distance,
      const std::vector<double>& origin_x_offset,
      const std::vector<double>& origin_y_offset,
      const std::vector<double>& origin_z_offset)>;
  using GetIntrinsicsCallback2 = std::function<bool(
      std::vector<double>& elevation, std::vector<double>& azimuth,
      std::vector<double>& distance, std::vector<double>& origin_x_offset,
      std::vector<double>& origin_y_offset,
      std::vector<double>& origin_z_offset)>;
  using GetLaserIdCallback = std::function<int(size_t idx)>;
  LidarIntrinsics(std::shared_ptr<DeviceContext> device_context, int laser_num);
  ~LidarIntrinsics() = default;
  bool initFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config);
  void registerSetIntrinsicsCallback(SetIntrinsicsCallback cb);
  void registerSetIntrinsicsCallback(SetIntrinsicsCallback2 cb);
  void registerGetIntrinsicsCallback(GetIntrinsicsCallback cb);
  void registerGetIntrinsicsCallback(GetIntrinsicsCallback2 cb);
  void registerGetLaserIdCallback(GetLaserIdCallback cb);
  // for manual calibration
  void update(PointCloudT::Ptr cloud);

 private:
  bool setIntrinsics();
  bool getIntrinsics();
  void loadIntrinsics(const std::string& filename);
  void saveIntrinsics(const std::string& filename);
  void updateCalibWidgetValue();

 private:
  std::shared_ptr<DeviceContext> device_context_;
  // property namespace
  std::string namespace_{"lidar_intrinsics"};
  // property widgets
  std::shared_ptr<QPushButton> load_intrinsics_button_;
  std::shared_ptr<QLabel> label_intrinsics_file_;
  std::shared_ptr<QPushButton> save_intrinsics_button_;
  std::shared_ptr<QCheckBox> checkbox_auto_load_;
  // property widgets for manual calibration
  std::shared_ptr<QCheckBox> checkbox_enable_calib_;
  std::shared_ptr<QCheckBox> checkbox_enable_calib_elevation_;
  std::shared_ptr<QCheckBox> checkbox_enable_calib_azimuth_;
  std::shared_ptr<QCheckBox> checkbox_enable_calib_distance_;
  std::shared_ptr<QCheckBox> checkbox_enable_calib_origin_;
  std::shared_ptr<QCheckBox> checkbox_refine_mode_;
  std::shared_ptr<QSpinBox> spinbox_laser_id_;
  std::shared_ptr<QDoubleSpinBox> spinbox_elevation_offset_;
  std::shared_ptr<QDoubleSpinBox> spinbox_azimuth_offset_;
  std::shared_ptr<QDoubleSpinBox> spinbox_distance_offset_;
  std::shared_ptr<QDoubleSpinBox> spinbox_origin_x_offset_;
  std::shared_ptr<QDoubleSpinBox> spinbox_origin_y_offset_;
  std::shared_ptr<QDoubleSpinBox> spinbox_origin_z_offset_;
  // data
  size_t laser_num_;
  std::vector<double> elevation_offset_;
  std::vector<double> azimuth_offset_;
  std::vector<double> distance_offset_;
  std::vector<double> origin_x_offset_;
  std::vector<double> origin_y_offset_;
  std::vector<double> origin_z_offset_;
  std::string load_filepath_;
  // callback
  SetIntrinsicsCallback set_intrinsics_cb_;
  GetIntrinsicsCallback get_intrinsics_cb_;
  SetIntrinsicsCallback2 set_intrinsics_cb2_;
  GetIntrinsicsCallback2 get_intrinsics_cb2_;
  GetLaserIdCallback get_laser_id_cb_;
  // for manual calibration
  bool enable_calib_{false};
  bool enable_calib_elevation_{false};
  bool enable_calib_azimuth_{false};
  bool enable_calib_distance_{false};
  bool enable_calib_origin_{false};
  int current_laser_id_{0};
  double angle_step_{0.1};
  double distance_step_{0.01};
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTCLOUD_LIDAR_INTRINSICS_H
