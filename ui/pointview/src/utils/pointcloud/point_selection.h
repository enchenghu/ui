//
// Created by tongdayang on 1/10/23.
//

#ifndef POINTVIEW_POINT_SELECTION_H
#define POINTVIEW_POINT_SELECTION_H

#include <QPushButton>
#include <functional>
#include <memory>
#include <set>

#include "point_table.h"
#include "pointcloud.h"
#include "utils/common/device_context.h"
#include "utils/property_tree.h"
#include "utils/xqtwidget.h"

namespace autox {
namespace pointview {

class PointSelection : public PointTable {
  Q_OBJECT
 public:
  using PointInfoCallback =
      std::function<bool(size_t idx, std::vector<double>&)>;
  using PointUidCallback = std::function<bool(size_t idx, int&)>;
  PointSelection(std::shared_ptr<DeviceContext> device_context,
                 QStringList head = {});
  ~PointSelection();
  void setPointInfoCallback(PointInfoCallback cb) { point_info_cb_ = cb; };
  void setPointUidCallback(PointUidCallback cb) { point_uid_cb_ = cb; };
  void update(PointCloudT::Ptr cloud, bool refresh_table = true);
  void refreshTable();

 private:
  void markPoint(PointT& p);

 private slots:
  void on_openButtonClicked();
  void on_trackingModeClicked(int state);

 private:
  PropertyPairs properties_;
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
  std::shared_ptr<XCheckBox> track_mode_checkbox_;
  bool is_track_mode_{false};
  bool need_update_flag_{false};
  std::vector<int> indices_;
  std::set<int> track_uids_;
  std::vector<int> valid_indices_;
  // for new api
  PointInfoCallback point_info_cb_;
  PointUidCallback point_uid_cb_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_POINT_SELECTION_H
