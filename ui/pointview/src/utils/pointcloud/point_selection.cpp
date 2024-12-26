//
// Created by tongdayang on 1/10/23.
//

#include "point_selection.h"

#include <glog/logging.h>

namespace autox {
namespace pointview {
PointSelection::PointSelection(std::shared_ptr<DeviceContext> device_context,
                               QStringList head)
    : PointTable(device_context->getDisplayContext()->getParent(), head,
                 "Point Selection"),
      open_button_(new QPushButton(this)) {
  device_context_ = device_context;
  track_uids_.clear();
  // register callback
  device_context_->registerPointSelectionCb(
      [this](const std::vector<int>& indices) {
        indices_ = indices;
        if (indices_.size() > 0) {
          need_update_flag_ = true;
          device_context_->refreshPointCloud();
        }
      });
  // window ui
  track_mode_checkbox_ = std::make_shared<XCheckBox>("track mode:");
  custom_function_layout_->addWidget(track_mode_checkbox_.get());
  connect(track_mode_checkbox_->getWidget().get(), SIGNAL(stateChanged(int)),
          this, SLOT(on_trackingModeClicked(int)));
  // properties
  open_button_->setText("Open");
  device_context_->addProperty("Point Selection", open_button_);
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  device_context_->addProperties(properties_);
}

PointSelection::~PointSelection() {}

void PointSelection::markPoint(PointT& p) {
  p.r = 255;
  p.g = 0;
  p.b = 0;
}

void PointSelection::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void PointSelection::on_trackingModeClicked(int state) {
  is_track_mode_ = (state > 0);
}

void PointSelection::update(PointCloudT::Ptr cloud, bool refresh_table) {
  if (need_update_flag_) {
    valid_indices_.clear();
    track_uids_.clear();
    // ignore hided points
    int uid;
    for (auto& i : indices_) {
      if (i < cloud->points.size() && cloud->points[i].a != 0) {
        valid_indices_.push_back(i);
        if (point_uid_cb_ && point_uid_cb_(i, uid)) {
          track_uids_.insert(uid);
        }
      }
    }
    LOG(INFO) << "selected point number:" << valid_indices_.size();
    // update table and mark points
    if (valid_indices_.size() == 0 || !point_info_cb_) {
      return;
    }
    for (auto& i : valid_indices_) {
      if (i >= 0 && i < cloud->points.size()) {
        markPoint(cloud->points[i]);
      } else {
        LOG(INFO) << "point_selection out of range:" << i;
      }
    }
    if (refresh_table) {
      refreshTable();
    }
    need_update_flag_ = false;
    return;
  }
  // tracking mode
  if (is_track_mode_ && device_context_->getPlayerState().is_playing) {
    if (track_uids_.size() == 0 || !point_info_cb_ || !point_uid_cb_) {
      return;
    }
    valid_indices_.clear();
    int uid;
    for (int i = 0; i < cloud->points.size(); i++) {
      if (point_uid_cb_(i, uid) && track_uids_.find(uid) != track_uids_.end()) {
        valid_indices_.push_back(i);
        markPoint(cloud->points[i]);
      }
    }
  }
  if (refresh_table) {
    refreshTable();
  }
}

void PointSelection::refreshTable() {
  cleanTable();
  std::vector<double> data;
  for (auto& i : valid_indices_) {
    data.clear();
    if (point_info_cb_(i, data)) {
      addPoint(data);
    }
  }
  refresh();
}

}  // namespace pointview
}  // namespace autox
