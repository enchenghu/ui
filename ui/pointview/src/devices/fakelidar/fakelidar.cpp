#include "fakelidar.h"

#include "ui_fakelidarsetting.h"

FakeLidar::FakeLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
                     autox::pointview::DeviceBaseParameter& parameter)
    : autox::pointview::DeviceBase(context, parameter),
      viewer_(context->getViewerPtr()),
      point_number_label_(new QLabel),
      cloud_(new PointCloudT) {
  // add point cloud to pcl viewer
  viewer_->addPointCloud(cloud_,
                         std::to_string(device_context_->getDeviceId()));
  // UI setting
  // Property tree setting
  // point cloud manipulator
  std::vector<autox::pointview::ChannelSetting> channel_settings;
  channel_settings.push_back({"none", "gray", 0, 100});
  channel_settings.push_back({"x", "jet", 0, 100});
  channel_settings.push_back({"y", "jet", 0, 100});
  channel_settings.push_back({"z", "jet", 0, 100});
  channel_settings.push_back({"distance", "jet", 0, 100});
  channel_settings.push_back({"intensity", "jet", 0, 255});
  manipulator_ = std::make_shared<autox::pointview::PointManipulator>(
      device_context_, channel_settings);
  // point cloud info
  device_context_->addProperty("number", point_number_label_);
  // Fake lidar generate
  point_cloud_ = std::make_shared<PointCloud>();
  faker_point_generator_ = std::make_shared<FakerPointGenerator>();
  faker_point_generator_->setPointCloudCallback(
      [this](std::shared_ptr<PointCloud>& point_cloud) {
        std::lock_guard<std::mutex>(this->lock);
        if (!device_context_->getRefreshState()) {
          point_cloud_.swap(point_cloud);
          LOG(INFO) << "points size: " << point_cloud_->points.size();
          device_context_->refreshPointCloud();
        } else {
          LOG(INFO) << "drop a point cloud frame";
        }
      });
  faker_point_generator_->createFloorScan(2, 360);
  // Create thread
  using namespace std::chrono_literals;
  lidar_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playing) {
        // generate points
        auto start = std::chrono::steady_clock::now();
        faker_point_generator_->generate(0.05, 0.3);
        auto end = std::chrono::steady_clock::now();
        LOG(INFO) << "Generate time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - start)
                         .count()
                  << " ms";
        auto diff = 100ms - (end - start);
        if (diff > 0ms) {
          std::this_thread::sleep_for(diff);
        } else {
          LOG(INFO) << "over";
        }
      } else {
        std::this_thread::sleep_for(100ms);
      }
    }
  });
}

FakeLidar::~FakeLidar() {
  exit_ = true;
  if (lidar_thread_->joinable()) {
    lidar_thread_->join();
  }
  viewer_->removePointCloud(std::to_string(device_context_->getDeviceId()));
};

inline double getChannelValue(const Point& p, int channel_idx) {
  if (channel_idx == 0) {
    return 100;
  } else if (channel_idx == 1) {
    // x
    return fabs(p.x);
  } else if (channel_idx == 2) {
    // y
    return fabs(p.y);
  } else if (channel_idx == 3) {
    // z
    return fabs(p.z);
  } else if (channel_idx == 4) {
    // dis
    return p.distance;
  } else if (channel_idx == 5) {
    // intensity
    return p.intensity;
  }
  return 0;
}

PointT FakeLidar::convertToPclPoint(size_t i, int channel_idx) {
  double v;
  std::uint8_t r, g, b;
  PointT p2;
  auto& p = point_cloud_->points[i];
  if (channel_idx == 0) {
    v = 100;
  } else if (channel_idx == 1) {
    // x
    v = fabs(p.x);
  } else if (channel_idx == 2) {
    // y
    v = fabs(p.y);
  } else if (channel_idx == 3) {
    // z
    v = fabs(p.z);
  } else if (channel_idx == 4) {
    // dis
    v = p.distance;
  } else if (channel_idx == 5) {
    // intensity
    v = p.intensity;
  }
  // v -> [0, 255]
  manipulator_->transformColor(v, r, g, b);
  p2.x = p.x;
  p2.y = p.y;
  p2.z = p.z;
  p2.r = r;
  p2.g = g;
  p2.b = b;
  return p2;
}

bool FakeLidar::updateUI() {
  std::lock_guard<std::mutex>(this->lock);
  if (!device_context_->getRefreshState()) {
    return false;
  }
  auto start = std::chrono::steady_clock::now();
  cloud_->resize(point_cloud_->points.size());
  // convert the raw point cloud (raw_point_cloud_) to pcl point cloud (cloud_)
  int channel_idx = manipulator_->getChannelIndex();
  for (size_t i = 0; i < point_cloud_->points.size(); i++) {
    cloud_->points[i] = convertToPclPoint(i, channel_idx);
  }
  // update point cloud in viewer
  std::string cloud_id = std::to_string(device_context_->getDeviceId());
  viewer_->updatePointCloud(cloud_, cloud_id);
  manipulator_->update(cloud_id);
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  LOG(INFO) << "update time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << " ms";
  point_number_label_->setText(QString::number(point_cloud_->points.size()));
  return true;
}
