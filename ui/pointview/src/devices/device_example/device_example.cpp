#include "device_example.h"

DeviceExample::DeviceExample(
    std::shared_ptr<autox::pointview::DisplayContext> context, int device_id,
    const std::string& device_name)
    : autox::pointview::DeviceBase(context, device_id, device_name),
      viewer_(context->getViewerPtr()),
      cloud_(new PointCloudT) {
  // add point cloud to pcl viewer
  viewer_->addPointCloud(cloud_,
                         std::to_string(device_context_->getDeviceId()));
}

DeviceExample::~DeviceExample() {
  // remove point cloud from pcl viewer
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
}

bool DeviceExample::updateUI() {
  // generate random cloud
  cloud_->resize(10000);
  for (int i = 0; i < cloud_->points.size(); ++i) {
    // random position
    cloud_->points[i].x = ((float)rand() / RAND_MAX) * 100;
    cloud_->points[i].y = ((float)rand() / RAND_MAX) * 100;
    cloud_->points[i].z = ((float)rand() / RAND_MAX) * 100;
    // set red color
    cloud_->points[i].r = 255;
    cloud_->points[i].g = 0;
    cloud_->points[i].b = 0;
  }
  // update point cloud
  viewer_->updatePointCloud(cloud_,
                            std::to_string(device_context_->getDeviceId()));
  // true to call engine to update viewer
  return true;
}
