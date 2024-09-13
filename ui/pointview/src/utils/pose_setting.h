/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POSE_SETTING_H
#define POSE_SETTING_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/pointcloud/pointcloud.h"

namespace autox {
namespace pointview {

class PoseSetting : public QObject {
  Q_OBJECT

 public:
  PoseSetting(std::shared_ptr<DeviceContext> device_context);
  ~PoseSetting() = default;
  bool initFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config);
  void transform(PointCloudT::Ptr cloud);

 private:
  void updatePoseSetting();

 private:
  std::shared_ptr<DeviceContext> device_context_;
  // item
  std::shared_ptr<QCheckBox> checkbox_enable_;
  std::vector<std::shared_ptr<QDoubleSpinBox>> spinbox_translations_;
  std::vector<std::shared_ptr<QDoubleSpinBox>> spinbox_rotations_;
  std::shared_ptr<QCheckBox> checkbox_use_euler_;
  std::vector<std::shared_ptr<QDoubleSpinBox>> spinbox_euler_rpy_;
  // data
  std::string namespace_{"pose"};
  bool enable_{false};
  bool use_euler_angle_{false};
  Eigen::Matrix4f pose_ = Eigen::Matrix4f::Identity();
};

}  // namespace pointview
}  // namespace autox

#endif  // POSE_SETTING_H
