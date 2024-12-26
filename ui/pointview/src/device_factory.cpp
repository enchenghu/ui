/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "device_factory.h"

#include "devices/blidar/blidar.h"
#include "devices/fakelidar/fakelidar.h"
#include "devices/hesai_ft120/hesai_ft120.h"
#include "devices/hesai_pandar128/hesai_pandar128.h"
#include "devices/hesai_qt128/hesai_qt128.h"
#include "devices/hesai_qt64/hesai_qt64.h"
#include "devices/udp_replayer/udp_replayer.h"
#include "devices/xlidar/xlidar.h"
#include "devices/xradar/xradar.h"
#include "devices/xradar_2d/xradar_2d.h"
#include "devices/xrt_replayer/xrt_camera.h"
#include "devices/xrt_replayer/xrt_lidar.h"
#include "devices/xrt_replayer/xrt_replayer.h"

namespace autox {
namespace pointview {

DeviceFactory::DeviceFactory() {
  RegisterDevice<autox::drivers::xlidar::XLidar>("XLidar");
  RegisterDevice<autox::drivers::blidar::BLidar>("BLidar");
  RegisterDevice<autox::drivers::xradar_2d::XRadar2D>("XRadar2D");
  RegisterDevice<UdpReplayer>("UdpReplayer");
  RegisterDevice<XRadar>("xRadar");
  RegisterDevice<HesaiPandar128>("HesaiPandar128");
  RegisterDevice<HesaiQt128>("HesaiQt128");
  RegisterDevice<HesaiQt64>("HesaiQt64");
  RegisterDevice<HesaiFT120>("HesaiFt120");
  RegisterDevice<XrtReplayer>("XrtReplayer");
  RegisterDevice<XrtLidar>("XrtLidar");
  RegisterDevice<XrtCamera>("XrtCamera");
  RegisterDevice<FakeLidar>("FakeLidar");
}

std::shared_ptr<DeviceBase> DeviceFactory::createDevice(
    const std::string& type, std::shared_ptr<DisplayContext> context,
    int device_id, const std::string& device_name) {
  auto it = products_.find(type);
  if (it == products_.end()) {
    return nullptr;
  }
  auto& creator = it->second;
  DeviceBaseParameter para{device_id, device_name, type};
  return creator(context, para);
}

std::vector<std::string> DeviceFactory::getDeviceTypes() {
  return device_types_;
}

bool DeviceFactory::AddCreator(const std::string& type, Creator func) {
  if (products_.insert(std::make_pair(type, func)).second) {
    device_types_.push_back(type);
  }
  return true;
}

bool DeviceFactory::unregisterDevice(const std::string& type) {
  return products_.erase(type) == 1;
}

}  // namespace pointview
}  // namespace autox
