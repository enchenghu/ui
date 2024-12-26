/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_DEVICE_FACTORY_H
#define POINTVIEW_DEVICE_FACTORY_H

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "utils/common/device_base.h"

namespace autox {
namespace pointview {

class DeviceFactory {
  using Creator = std::function<std::shared_ptr<DeviceBase>(
      std::shared_ptr<DisplayContext>, DeviceBaseParameter&)>;

 public:
  DeviceFactory();
  std::shared_ptr<DeviceBase> createDevice(
      const std::string& type, std::shared_ptr<DisplayContext> context,
      int device_id, const std::string& device_name);
  std::vector<std::string> getDeviceTypes();

 private:
  bool AddCreator(const std::string& type, Creator func);
  template <typename T>
  bool RegisterDevice(const std::string& type) {
    Creator func = [](std::shared_ptr<DisplayContext> context,
                      DeviceBaseParameter& para) {
      return std::make_shared<T>(context, para);
    };
    return AddCreator(type, func);
  }
  bool unregisterDevice(const std::string& type);

 private:
  std::map<std::string, Creator> products_;
  std::vector<std::string> device_types_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_DEVICE_FACTORY_H
