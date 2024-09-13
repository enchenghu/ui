//
// Created by tongdayang on 2/2/23.
//

#include "app_control.h"

namespace autox {
namespace pointview {
AppControl::AppControl()
    : device_factory_(new DeviceFactory()),
      init_camera_position_({0, 0, 40, 0, 0, 0, 0, 1, 0}) {
  // init device type
  auto device_types = device_factory_->getDeviceTypes();
  for (auto& type : device_types) {
    device_types_ << QString::fromStdString(type);
  }
  // init current device
  current_player_device_.reset();
}

AppControl::~AppControl() {}

void AppControl::selectPointCloudCallback(
    const pcl::visualization::AreaPickingEvent& event, void* userData) {
  std::vector<int> indices;
  if (event.getPointsIndices(indices)) {
    for (auto device : device_list_) {
      if (device->getDeviceContext()->getPlayerState().is_playing) {
        QMessageBox msgBox;
        msgBox.setText("Error.");
        msgBox.setInformativeText(
            "Can not select points when it's playing.\n Please pause and then "
            "select.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
      }
    }
    for (auto device : device_list_) {
      device->getDeviceContext()->updatePointSelection(indices);
    }
  } else {
    std::cout << "Empty" << std::endl;
  }
}

void AppControl::setCameraPose(AppControl::CameraPose pose) {
  auto viewer = display_context_->getViewerPtr();
  switch (pose) {
    case CameraPose::Front: {
      viewer->setCameraPosition(-20, 0, 0, 0, 0, 0, 0, 0, 1);
      break;
    }
    case CameraPose::Top: {
      viewer->setCameraPosition(0, 0, 20, 0, 0, 0, 1, 0, 0);
      break;
    }
    case CameraPose::Init: {
      viewer->setCameraPosition(
          init_camera_position_[0], init_camera_position_[1],
          init_camera_position_[2], init_camera_position_[3],
          init_camera_position_[4], init_camera_position_[5],
          init_camera_position_[6], init_camera_position_[7],
          init_camera_position_[8]);
      break;
    }
    case CameraPose::Left: {
      viewer->setCameraPosition(0, 20, 0, 0, 0, 0, 0, 0, 1);
      break;
    }
    case CameraPose::Right: {
      viewer->setCameraPosition(0, -20, 0, 0, 0, 0, 0, 0, 1);
      break;
    }
    default: {
      std::cout << "un-support pose setting." << std::endl;
    }
  }
}

int AppControl::addDevice(std::string device_type,
                          std::shared_ptr<Config> config,
                          const std::string& device_name) {
  auto device = device_factory_->createDevice(device_type, display_context_,
                                              unique_device_id_, device_name);
  if (device == nullptr) {
    // unknown device
    std::cout << "unknown device:" << device_type << std::endl;
    return -1;
  }
  if (config != nullptr) {
    device->initFromConfig(config);
  }
  device_type_map_.insert({unique_device_id_, device_type});
  // show setting ui
  //  device->move(this->geometry().center() - device->rect().center());
  //  device->show();
  // add to devices
  device_list_.push_back(device);
  unique_device_id_++;
  return device->getDeviceContext()->getDeviceId();
}

void AppControl::Init(std::shared_ptr<DisplayContext> display_context) {
  display_context_ = display_context;
  auto viewer = display_context_->getViewerPtr();
  // register point pick event
  viewer->registerAreaPickingCallback(&AppControl::selectPointCloudCallback,
                                      *this);
}

QStringList AppControl::getDeviceTypes() { return device_types_; }

int AppControl::isValidTreeItem(QTreeWidgetItem* item) {
  for (unsigned int i = 0; i < device_list_.size(); i++) {
    auto property_tree = device_list_[i]->getDeviceContext()->getPropertyTree();
    if (property_tree->getTopItem().get() == item) {
      return device_list_[i]->getDeviceContext()->getDeviceId();
    }
  }
  return -1;
}

bool AppControl::removeDevice(int device_id) {
  for (unsigned int i = 0; i < device_list_.size(); i++) {
    if (device_list_[i]->getDeviceContext()->getDeviceId() == device_id) {
      // remove device
      current_player_device_.reset();
      device_list_.erase(device_list_.begin() + i);
      return true;
    }
  }
  return false;
}

bool AppControl::setCurrentPlayer(int device_id) {
  // reset signal and slot connection
  if (current_player_device_) {
    disconnect(current_player_device_.get(), SIGNAL(SyncPlayer()), this,
               SLOT(PlayerStateChange()));
  }
  // find current device
  current_player_device_ = nullptr;
  for (unsigned int i = 0; i < device_list_.size(); i++) {
    if (device_list_[i]->getDeviceContext()->getDeviceId() == device_id) {
      current_player_device_ = device_list_[i];
      connect(current_player_device_.get(), SIGNAL(SyncPlayer()), this,
              SLOT(PlayerStateChange()));
      break;
    }
  }
  return !(current_player_device_ == nullptr);
}

bool AppControl::readyToSelect() {
  for (auto device : device_list_) {
    auto play_state = device->getDeviceContext()->getPlayerState();
    if (play_state.is_playing) {
      return false;
    }
  }
  return true;
}

bool AppControl::readyToUpdate() {
  bool dirty = false;
  bool flag;
  for (size_t i = 0; i < device_list_.size(); i++) {
    auto& device = device_list_[i];
    flag = device->updateUI();
    dirty |= flag;
  }
  return dirty;
}

int AppControl::getCurrentPlayerProgress() {
  if (current_player_device_) {
    auto state = current_player_device_->getDeviceContext()->getPlayerState();
    if (state.is_playback) {
      return state.current_frame_idx;
    }
  }
  return -1;
}

bool AppControl::getRecordState() {
  if (current_player_device_) {
    auto device_context = current_player_device_->getDeviceContext();
    return device_context->getPlayerState().is_recording;
  }
  return false;
}
std::string AppControl::getRecordPath() {
  if (current_player_device_) {
    auto device_context = current_player_device_->getDeviceContext();
    return device_context->getPlayerState().record_path;
  } else {
    return std::string("");
  }
}
bool AppControl::playingStateChange() {
  if (current_player_device_) {
    auto device_context = current_player_device_->getDeviceContext();
    PlayerCmd cmd;
    if (device_context->getPlayerState().is_playing) {
      cmd.type = PlayerCmdType::PausePlayer;
    } else {
      cmd.type = PlayerCmdType::StartPlayer;
    }
    current_player_device_->updatePlayerState(cmd);
    return device_context->getPlayerState().is_playing;
  }
  return false;
}

bool AppControl::recordStateChange() {
  if (current_player_device_) {
    auto device_context = current_player_device_->getDeviceContext();
    PlayerCmd cmd;
    if (device_context->getPlayerState().is_recording) {
      cmd.type = PlayerCmdType::StopRecorder;
    } else {
      cmd.type = PlayerCmdType::StartRecorder;
    }
    device_context->updatePlayerState(cmd);
    return device_context->getPlayerState().is_recording;
  }
  return false;
}

void AppControl::PlayerStateChange() { emit SyncPlayerState(); }

std::shared_ptr<PlayerState> AppControl::getCurrentPlayerState() {
  if (current_player_device_) {
    auto state = current_player_device_->getDeviceContext()->getPlayerState();
    return std::make_shared<PlayerState>(state);
  }
  return nullptr;
}

bool AppControl::resetPlayerProgress() {
  if (current_player_device_) {
    auto device_context = current_player_device_->getDeviceContext();
    // stop play
    if (device_context->getPlayerState().is_playing) {
      playingStateChange();
    }
    // reset play progress to zero
    PlayerCmd cmd;
    cmd.type = PlayerCmdType::JumpPlayback;
    cmd.frame_idx = 0;
    device_context->updatePlayerState(cmd);
  }
  return false;
}

void AppControl::setCurrentPlayerProgress(int value) {
  if (current_player_device_) {
    PlayerCmd cmd;
    cmd.type = PlayerCmdType::JumpPlayback;
    cmd.frame_idx = value;
    current_player_device_->getDeviceContext()->updatePlayerState(cmd);
  }
}

void AppControl::save2Config(std::shared_ptr<Config> config) {
  // save device config
  for (size_t i = 0; i < device_list_.size(); i++) {
    // device config
    auto device_conf = std::make_shared<Config>();
    int device_id = device_list_[i]->getDeviceContext()->getDeviceId();
    std::string device_type = device_type_map_[device_id];
    device_conf->setParameter("device_type", device_type);
    auto device_name = device_list_[i]->getDeviceContext()->getDeviceName();
    device_conf->setParameter("device_name", device_name);
    device_list_[i]->storeToConfig(device_conf);
    // save config
    auto deivce_ns = getFullName("lidar_lists", std::to_string(device_id));
    config->saveSubConfig(deivce_ns, device_conf);
  }
  // save view point
  auto viewer = display_context_->getViewerPtr();
  pcl::visualization::Camera camera;
  viewer->getCameraParameters(camera);
  std::vector<float> view_point;
  for (int i = 0; i < 3; i++) {
    view_point.push_back(camera.pos[i]);
  }
  for (int i = 0; i < 3; i++) {
    view_point.push_back(camera.focal[i]);
  }
  for (int i = 0; i < 3; i++) {
    view_point.push_back(camera.view[i]);
  }
  config->setParameter("view_point", view_point);
}

void AppControl::setInitCameraPosition(std::vector<float>& view_point) {
  if (view_point.size() != 9) {
    return;
  }
  init_camera_position_.clear();
  init_camera_position_ = view_point;
}
}  // namespace pointview
}  // namespace autox