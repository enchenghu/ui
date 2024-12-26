
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "lidar_return_info.h"

#include <iostream>

#include <glog/logging.h>

namespace autox {
namespace pointview {

LidarReturnInfo::LidarReturnInfo(
    std::shared_ptr<DeviceContext> device_context,
    const std::vector<LidarReturnSetting>& return_settings)
    : QObject(device_context->getDisplayContext()->getParent()),
      device_context_(device_context),
      return_mode_selection_(new QComboBox) {
  // init return mode map
  for (auto& setting : return_settings) {
    if (setting.retrun_names.size() > 2) {
      LOG(INFO) << "only support single return and dual return mode!";
      continue;
    }
    std::vector<QString> retrun_names;
    for (auto& item : setting.retrun_names) {
      retrun_names.push_back(QString::fromStdString(item));
    }
    return_mode_map_.insert({setting.return_code, retrun_names});
  }
  // return mode selection
  return_mode_selection_->addItem("unknown");
  connect(return_mode_selection_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(ReturnModeSelectionChanged(int)));
  device_context_->addProperty("return mode", return_mode_selection_);
}

void LidarReturnInfo::update(uint8_t return_mode) {
  if (return_mode == return_mode_) {
    return;
  }
  return_mode_ = return_mode;
  auto it = return_mode_map_.find(return_mode);
  if (it == return_mode_map_.end()) {
    LOG(INFO) << "unknow return mode: " << return_mode;
    return_mode_selection_->clear();
    return_mode_selection_->addItem("unknown");
    return;
  }
  auto modes = it->second;
  if (modes.size() == 1) {
    // signle return
    return_mode_selection_->clear();
    return_mode_selection_->addItem(modes[0]);
  } else if (modes.size() == 2) {
    // dual return
    return_mode_selection_->clear();
    return_mode_selection_->addItem(modes[0] + " & " + modes[1]);
    return_mode_selection_->addItem(modes[0]);
    return_mode_selection_->addItem(modes[1]);
    return_mode_selection_->addItem("only " + modes[0]);
    return_mode_selection_->addItem("only " + modes[1]);
  } else {
    return;
  }
}

void LidarReturnInfo::ReturnModeSelectionChanged(int index) {
  return_mode_selection_index_ = index;
  device_context_->refreshPointCloud();
}

bool LidarReturnInfo::checkValid(uint8_t return_id) {
  if (return_mode_selection_index_ == 0) {
    return true;
  } else if (return_mode_selection_index_ == 1) {
    return return_id != 2;
  } else if (return_mode_selection_index_ == 2) {
    return return_id != 1;
  } else if (return_mode_selection_index_ == 3) {
    return return_id == 1;
  } else if (return_mode_selection_index_ == 4) {
    return return_id == 2;
  }
}

}  // namespace pointview
}  // namespace autox
