//
// Created by tongdayang on 1/5/23.
//

#ifndef POINTVIEW_LASER_TRACK_H
#define POINTVIEW_LASER_TRACK_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include <map>
#include <memory>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/pointcloud/point_table.h"
#include "utils/pointcloud/pointcloud.h"
#include "utils/property_tree.h"
#include "utils/xqtwidget.h"

namespace autox {
namespace pointview {

class LaserTrack : public PointTable {
  Q_OBJECT
 public:
  using PointInfoCallback =
      std::function<bool(size_t idx, std::vector<double>&)>;
  using PointCheckCallback = std::function<bool(size_t idx)>;
  LaserTrack(std::shared_ptr<DeviceContext> device_context,
             QStringList table_head);
  ~LaserTrack();
  bool initFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config);
  // for new api
  void setPointInfoCallback(PointInfoCallback cb) { point_info_cb_ = cb; };
  void setPointCheckCallback(PointCheckCallback cb) { point_check_cb_ = cb; };
  bool checkPoint(int ch, float azimuth);
  void update(PointCloudT::Ptr cloud);

 private:
  void markPoint(PointT& p);

 protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;

 private slots:
  void on_openButtonClicked();
  void on_saveRuleButtonClicked();

 private:
  void setRuletoSpinBox();

 private:
  PropertyPairs properties_;
  std::shared_ptr<DeviceContext> device_context_;
  std::string namespace_{"lidar_point"};
  std::shared_ptr<QPushButton> open_button_;
  bool update_{false};
  std::map<std::string, int> rules_;
  std::shared_ptr<XSpinBox> start_channel_widget_, end_channel_widget_;
  std::shared_ptr<XDoubleSpinBox> start_angle_widget_, end_angle_widget_;
  std::shared_ptr<QPushButton> save_rule_button_;
  // for new api
  PointInfoCallback point_info_cb_;
  PointCheckCallback point_check_cb_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_LASER_TRACK_H
