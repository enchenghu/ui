/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_DEVICE_BASE_H
#define POINTVIEW_DEVICE_BASE_H

#include <QLabel>
#include <QTreeWidgetItem>

#include "device_context.h"
#include "display_context.h"
#include "utils/config.h"
#include "utils/pointcloud/pointcloud.h"

namespace autox {
namespace pointview {

class DeviceBase : public QObject {
  Q_OBJECT
 public:
  enum class PlayerState { Start, Stop, Record, FinishRecord };

 public:
  explicit DeviceBase(std::shared_ptr<DisplayContext> context,
                      DeviceBaseParameter& parameter);
  virtual ~DeviceBase() = default;

 signals:
  void SyncPlayer();

 public:
  // point cloud interfaces
  virtual bool updateUI() { return false; };
  // config
  virtual bool initFromConfig(std::shared_ptr<Config> config) { return true; };
  virtual bool storeToConfig(std::shared_ptr<Config> config) { return false; };

  // device context
  std::shared_ptr<DeviceContext> getDeviceContext();
  // response to play state change
  virtual bool updatePlayerState(PlayerCmd cmd);

 protected:
  // device context
  std::shared_ptr<DeviceContext> device_context_;
};
}  // namespace pointview
}  // namespace autox
#endif  // POINTVIEW_DEVICE_BASE_H
