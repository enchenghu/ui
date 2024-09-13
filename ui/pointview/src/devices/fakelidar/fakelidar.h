#ifndef FAKELIDARSETTING_H
#define FAKELIDARSETTING_H

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <memory>
#include <mutex>
#include <thread>

#include "fake_point_generator.h"
#include "utils/common/device_base.h"
#include "utils/common/device_context.h"
#include "utils/pointcloud/point_manipulator.h"

namespace Ui {
class FakeLidarSetting;
}

class FakeLidar : public autox::pointview::DeviceBase {
  Q_OBJECT
 public:
  FakeLidar(std::shared_ptr<autox::pointview::DisplayContext> context,
            int device_id, const std::string& device_name);
  ~FakeLidar() override;
  bool updateUI() override;

 private:
  PointT convertToPclPoint(size_t i, int channel_idx);

 private:
  // visualizer ptr
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // Ui object
  std::vector<std::shared_ptr<QTableWidgetItem>> device_information_;
  std::vector<std::shared_ptr<QTableWidgetItem>> device_control_;
  // Property tree object
  std::shared_ptr<QLabel> point_number_label_;
  // point cloud tools
  PointCloudT::Ptr cloud_;
  std::shared_ptr<autox::pointview::PointManipulator> manipulator_;
  bool exit_{false};
  std::mutex lock;
  // fake lidar
  std::unique_ptr<std::thread> lidar_thread_;
  std::shared_ptr<FakerPointGenerator> faker_point_generator_;
  std::shared_ptr<PointCloud> point_cloud_;
};

#endif  // FAKELIDARSETTING_H
