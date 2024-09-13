#ifndef SENSOREXAMPLE_H
#define SENSOREXAMPLE_H

#include "utils/common/device_base.h"

class DeviceExample : public autox::pointview::DeviceBase {
  Q_OBJECT
 public:
  // init function
  DeviceExample(std::shared_ptr<autox::pointview::DisplayContext> context,
                int device_id, const std::string& device_name);
  // deleter
  ~DeviceExample() override;
  // return the flag if the engine need update
  bool updateUI() override;

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // point cloud
  PointCloudT::Ptr cloud_;
};

#endif  // SENSOREXAMPLE_H
