/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTCLOUD_LIDAR_RETURN_INFO_H
#define POINTCLOUD_LIDAR_RETURN_INFO_H

#include <QComboBox>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"

namespace autox {
namespace pointview {

struct LidarReturnSetting {
  uint8_t return_code;
  std::vector<std::string> retrun_names;
};

class LidarReturnInfo : public QObject {
  Q_OBJECT

 public:
  LidarReturnInfo(std::shared_ptr<DeviceContext> device_context,
                  const std::vector<LidarReturnSetting>& return_settings);
  ~LidarReturnInfo() = default;

 public:
  void update(uint8_t return_mode);
  bool checkValid(uint8_t return_id);

 private slots:
  void ReturnModeSelectionChanged(int index);

 private:
  //
  std::shared_ptr<DeviceContext> device_context_;
  PropertyPairs properties_;
  std::map<uint8_t, std::vector<QString>> return_mode_map_;
  uint8_t return_mode_{0};
  int return_mode_selection_index_{0};
  bool updated_{false};
  // Property tree object
  std::shared_ptr<QComboBox> return_mode_selection_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTCLOUD_LIDAR_RETURN_INFO_H
