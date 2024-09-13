
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "device_base.h"

namespace autox {
namespace pointview {

DeviceBase::DeviceBase(std::shared_ptr<DisplayContext> context, int device_id,
                       const std::string& device_name)
    : QObject(context->getParent()) {
  device_context_ =
      std::make_shared<DeviceContext>(this, context, device_id, device_name);
};

std::shared_ptr<DeviceContext> DeviceBase::getDeviceContext() {
  return device_context_;
}

bool DeviceBase::updatePlayerState(PlayerCmd cmd) {
  return device_context_->updatePlayerState(cmd);
}

}  // namespace pointview
}  // namespace autox
