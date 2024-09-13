/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTCLOUD_MANIPULATOR_H
#define POINTCLOUD_MANIPULATOR_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "utils/colormap.h"
#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/visualization/pcl_visualizer.h"

namespace autox {
namespace pointview {

struct ChannelSetting {
  std::string channel_name;
  std::string default_colormap_name;
  double default_range_min;
  double default_range_max;
};

class ResizeLabel : public QLabel {
  Q_OBJECT
 public:
  ResizeLabel(QWidget* parent = nullptr) : QLabel(parent) {}

 signals:
  void Resize();

 protected:
  void resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    emit Resize();
  }
};
class PointManipulator : public QObject {
  Q_OBJECT

 public:
  PointManipulator(std::shared_ptr<DeviceContext> device_context,
                   std::vector<ChannelSetting> channel_settings);
  ~PointManipulator() = default;
  bool InitFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool StoreToConfig(std::shared_ptr<autox::pointview::Config> config);
  int getChannelIndex() { return channel_idx_; };
  inline void transformColor(double v, uint8_t& r, uint8_t& g, uint8_t& b) {
    colormap_->get(v, r, g, b);
  }
  std::shared_ptr<Colormap> getColormap() { return colormap_; }
  // for point hide/size
  void update(const std::string& cloud_id);

 signals:
  void ColorMapRangeChanged(std::string channel, int min, int max);

 private slots:
  void pointCloudChannelChanged(int index);
  void pointCloudRangeChanged();
  void pointCloudColormapChanged();

 private:
  //
  std::shared_ptr<DeviceContext> device_context_;
  autox::visualization::PCLVisualizer::Ptr viewer_;
  std::string namespace_{"point_setting"};
  // item
  std::shared_ptr<QComboBox> channel_selection_;
  std::shared_ptr<QDoubleSpinBox> range_min_spinbox_;
  std::shared_ptr<QDoubleSpinBox> range_max_spinbox_;
  std::shared_ptr<QComboBox> colormap_selection_;
  std::shared_ptr<QSpinBox> spinbox_point_size_;
  std::shared_ptr<QCheckBox> checkbox_hide_;
  // colormap setting
  std::shared_ptr<Colormap> colormap_;
  std::map<std::string, int> colormap_idx_map_;
  std::shared_ptr<ResizeLabel> colormap_label_;
  // channel setting
  std::vector<ChannelSetting> channel_settings_;
  int channel_idx_{0};
  bool is_hided_{false};
  int point_size_{1};
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTCLOUD_MANIPULATOR_H
