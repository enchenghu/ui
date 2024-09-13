/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "device_context.h"

namespace autox {
namespace pointview {

DeviceContext::DeviceContext(QObject* parent,
                             std::shared_ptr<DisplayContext> display_context,
                             int device_id, const std::string& device_name) {
  parent_ = parent;
  device_id_ = device_id;
  display_context_ = display_context;
  device_name_ = device_name;
  device_property_tree_ = display_context->createDevicePropertyTree(
      QString::fromStdString(device_name));
  // add device_id in device property_tree
  device_id_label_ = std::make_shared<QLabel>();
  device_id_label_->setText(QString::number(device_id_));
  addProperty("id", device_id_label_);
  // initialize play state
  player_state_.is_playback = false;
  player_state_.is_playing = false;
  player_state_.is_recording = false;
  player_state_.total_frames = 0;
  player_state_.current_frame_idx = 0;
  player_state_.need_playback_reload = false;
  player_state_.need_playback_jumb = false;
  player_state_.jump_frame_idx = 0;
  player_state_.record_fi = false;
  player_state_.record_wait = false;
  player_state_.rate_record = 0.0;
};

QObject* DeviceContext::getParent() { return parent_; }

std::shared_ptr<DisplayContext> DeviceContext::getDisplayContext() {
  return display_context_;
};

int DeviceContext::getDeviceId() { return device_id_; }

void DeviceContext::refreshPointCloud() { refresh_ = true; }

bool DeviceContext::getRefreshState() { return refresh_; }

void DeviceContext::resetRefreshState() { refresh_ = false; }

void DeviceContext::registerPointSelectionCb(PointSelectionCallback func) {
  point_selection_funcs_.push_back(func);
}

void DeviceContext::updatePointSelection(const std::vector<int>& indices) {
  for (auto& cb : point_selection_funcs_) {
    cb(indices);
  }
}

std::shared_ptr<PropertyTree> DeviceContext::getPropertyTree() {
  return device_property_tree_;
}

void DeviceContext::addProperty(const QString& property,
                                std::shared_ptr<QWidget> value) {
  device_property_tree_->addProperty(property, value);
}

void DeviceContext::addProperties(const PropertyPairs& properties) {
  device_property_tree_->addProperties(properties);
}

void DeviceContext::registerStartPlayerCb(TriggerCallback func) {
  start_player_funcs_.push_back(func);
}
void DeviceContext::registerPausePlayerCb(TriggerCallback func) {
  pause_player_funcs_.push_back(func);
}
void DeviceContext::registerStartRecorderCb(TriggerCallback func) {
  start_recorder_funcs_.push_back(func);
}
void DeviceContext::registerStopRecorderCb(TriggerCallback func) {
  stop_recorder_funcs_.push_back(func);
}

bool DeviceContext::updatePlayerState(PlayerCmd cmd) {
  if (cmd.type == PlayerCmdType::StartPlayer) {
    // start player
    if (player_state_.is_playing) {
      return false;
    }
    for (auto& cb : start_player_funcs_) {
      cb();
    }
    player_state_.is_playing = true;
    return true;
  } else if (cmd.type == PlayerCmdType::PausePlayer) {
    // pause player
    if (!player_state_.is_playing || player_state_.is_recording) {
      return false;
    }
    for (auto cb : pause_player_funcs_) {
      cb();
    }
    player_state_.is_playing = false;
    return true;
  } else if (cmd.type == PlayerCmdType::StartRecorder) {
    // start recorder
    if (!player_state_.is_playing || player_state_.is_recording) {
      return false;
    }
    for (auto& cb : start_recorder_funcs_) {
      cb();
    }
    player_state_.is_recording = true;
    return true;
  } else if (cmd.type == PlayerCmdType::StopRecorder) {
    // stop recorder
    if (!player_state_.is_recording) {
      return false;
    }
    player_state_.is_recording = false;
    for (auto& cb : stop_recorder_funcs_) {
      cb();
    }
    return true;
  } else if (cmd.type == PlayerCmdType::JumpPlayback) {
    // jump frame of playback
    if (!player_state_.is_playback) {
      return false;
    }
    if (cmd.frame_idx < 0 || cmd.frame_idx >= player_state_.total_frames) {
      return false;
    }
    player_state_.need_playback_jumb = true;
    player_state_.jump_frame_idx = cmd.frame_idx;
    return true;
  } else if (cmd.type == PlayerCmdType::ReloadPlayback) {
    if (!player_state_.is_playback) {
      return false;
    }
    player_state_.need_playback_reload = true;
    return true;
  }
  return false;
}

void DeviceContext::updateRecordPath(std::string path) {
  player_state_.record_path = path;
}
void DeviceContext::updateCurrentFrame(int index) {
  player_state_.current_frame_idx = index;
}

void DeviceContext::updateTotalFrame(int n) {
  player_state_.total_frames = n;
  player_state_.current_frame_idx = 0;
}

void DeviceContext::updateJumpRate(double n) {
  player_state_.rate_playback_jump = n;
}
void DeviceContext::updateRecordRate(double n) {
  player_state_.rate_record = n;
}
void DeviceContext::updatePlayerType(bool is_playback) {
  player_state_.is_playback = is_playback;
}

void DeviceContext::updateRecordWait(bool n) {
  player_state_.record_wait = n;
}

void DeviceContext::resetPlaybackJump() {
  player_state_.need_playback_jumb = false;
}

void DeviceContext::resetPlaybackReload() {
  player_state_.need_playback_reload = false;
}

}  // namespace pointview
}  // namespace autox
