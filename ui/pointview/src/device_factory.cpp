/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "device_factory.h"

#include "devices/blidar/blidar.h"
#include "devices/device_example/device_example.h"
#include "devices/fakelidar/fakelidar.h"
#include "devices/hesai_ft120/hesai_ft120.h"
#include "devices/hesai_pandar128/hesai_pandar128.h"
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
  registerDevice<autox::drivers::xlidar::XLidar>("xLidar");
  registerDevice<autox::drivers::blidar::BLidar>("bLidar");
  registerDevice<XRadar>("xRadar");
  registerDevice<HesaiPandar128>("HesaiPandar128");
  registerDevice<HesaiQt64>("HesaiQt64");
  registerDevice<UdpReplayer>("UdpReplayer");
  registerDevice<XrtReplayer>("XrtReplayer");
  registerDevice<XrtLidar>("XrtLidar");
  registerDevice<XrtCamera>("XrtCamera");
  registerDevice<FakeLidar>("FakeLidar");
  registerDevice<DeviceExample>("DeviceExample");
  registerDevice<HesaiFT120>("HesaiFt120");
  registerDevice<autox::drivers::xradar_2d::XRadar2D>("XRadar2D");
}

std::shared_ptr<DeviceBase> DeviceFactory::createDevice(
    const std::string& type, std::shared_ptr<DisplayContext> context,
    int device_id, const std::string& device_name) {
  auto it = products_.find(type);
  if (it == products_.end()) {
    return nullptr;
  }
  auto& creator = it->second;
  return creator(context, device_id, device_name);
}

std::vector<std::string> DeviceFactory::getDeviceTypes() {
  return device_types_;
}

bool DeviceFactory::registerDevice(const std::string& type, Creator func) {
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
