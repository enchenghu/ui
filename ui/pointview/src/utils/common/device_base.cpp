
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "device_base.h"

namespace autox {
namespace pointview {

DeviceBase::DeviceBase(std::shared_ptr<DisplayContext> context,
                       DeviceBaseParameter& parameter)
    : QObject(context->getParent()) {
  device_context_ = std::make_shared<DeviceContext>(this, context, parameter);
};

std::shared_ptr<DeviceContext> DeviceBase::getDeviceContext() {
  return device_context_;
}

bool DeviceBase::updatePlayerState(PlayerCmd cmd) {
  return device_context_->updatePlayerState(cmd);
}

}  // namespace pointview
}  // namespace autox
