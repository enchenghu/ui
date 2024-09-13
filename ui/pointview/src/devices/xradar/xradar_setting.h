/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRADAR_SETTING_H
#define XRADAR_SETTING_H

#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"
#include "utils/config.h"

namespace autox {
namespace drivers {
namespace arbe {
class ArbeDriver;
}  // namespace arbe
}  // namespace drivers
}  // namespace autox

namespace autox {
namespace pointview {

class XRadarSetting : public QObject {
  Q_OBJECT

 public:
  XRadarSetting(std::shared_ptr<DeviceContext> device_context);
  ~XRadarSetting();
  bool initFromConfig(std::shared_ptr<Config> config);
  bool storeToConfig(std::shared_ptr<Config> config);

 private:
  void updateWidegets();
  bool connectTcp();

 private:
  //
  std::shared_ptr<DeviceContext> device_context_;
  // only for tcp connect
  std::shared_ptr<autox::drivers::arbe::ArbeDriver> driver_;
  // item
  std::shared_ptr<QLineEdit> edit_deivce_ip_;
  std::shared_ptr<QLineEdit> edit_deivce_port_;
  std::shared_ptr<QLabel> label_connect_status_;
  std::shared_ptr<QPushButton> btn_connect_;
  std::shared_ptr<QPushButton> btn_start_;
  std::shared_ptr<QPushButton> btn_stop_;
  std::shared_ptr<QSpinBox> edit_power_threshold_;
  std::shared_ptr<QSpinBox> edit_azimuth_threshold_;
  std::shared_ptr<QSpinBox> edit_elevation_threshold_;
  std::shared_ptr<QPushButton> btn_set_params_;
  // property namespace
  std::string namespace_{"xradar_setting"};
  // thread
  // std::unique_ptr<std::thread> tcp_thread_;
  // bool exit_{false};
  //
  bool connected_{false};
};

}  // namespace pointview
}  // namespace autox

#endif  // XRADAR_SETTING_H
