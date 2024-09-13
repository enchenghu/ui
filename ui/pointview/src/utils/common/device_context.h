/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_DEVICE_CONTEXT_H
#define POINTVIEW_DEVICE_CONTEXT_H

#include <QLabel>
#include <functional>

#include "display_context.h"

namespace autox {
namespace pointview {

struct PlayerState {
  bool is_playback;
  // play
  bool is_playing;
  int total_frames;
  int current_frame_idx;
  // record
  bool is_recording;
  double recorded_time;
  // playback
  bool need_playback_reload;
  bool need_playback_jumb;
  int jump_frame_idx;
  double rate_playback_jump;
  std::string record_path;
  double rate_record;
  bool record_fi;
  bool record_wait;
};

enum PlayerCmdType {
  StartPlayer,
  PausePlayer,
  StartRecorder,
  StopRecorder,
  JumpPlayback,
  ReloadPlayback
};

struct PlayerCmd {
  PlayerCmdType type;
  int frame_idx;
};

class DeviceContext {
  using TriggerCallback = std::function<void()>;
  using PointSelectionCallback = std::function<void(const std::vector<int>&)>;

 public:
  DeviceContext(QObject* parent,
                std::shared_ptr<DisplayContext> display_context, int device_id,
                const std::string& device_name = "unknown");
  QObject* getParent();
  std::shared_ptr<DisplayContext> getDisplayContext();
  int getDeviceId();
  std::string getDeviceName() { return device_name_; }
  // refresh
  void refreshPointCloud();
  bool getRefreshState();
  void resetRefreshState();
  // point selction
  void registerPointSelectionCb(PointSelectionCallback func);
  void updatePointSelection(const std::vector<int>& indices);
  // player fms
  void registerStartPlayerCb(TriggerCallback func);
  void registerPausePlayerCb(TriggerCallback func);
  void registerStartRecorderCb(TriggerCallback func);
  void registerStopRecorderCb(TriggerCallback func);
  // player interfaces
  bool updatePlayerState(PlayerCmd cmd);
  void updateCurrentFrame(int index);
  void updateTotalFrame(int n);
  void updateJumpRate(double n);
  void updateRecordRate(double n);
  void updateRecordFi(double is_fi) { player_state_.record_fi = is_fi; }
  void updatePlayerType(bool is_playback);
  void updateRecordWait(bool);
  void resetPlaybackJump();
  void resetPlaybackReload();
  PlayerState getPlayerState() { return player_state_; };
  void updateRecordPath(std::string path);
  // property
  std::shared_ptr<PropertyTree> getPropertyTree();
  void addProperty(const QString& property, std::shared_ptr<QWidget> value);
  void addProperties(const PropertyPairs& properties);

 private:
  QObject* parent_;
  std::shared_ptr<DisplayContext> display_context_;
  // unique device id
  int device_id_;
  std::shared_ptr<QLabel> device_id_label_;
  // property_tree
  std::shared_ptr<PropertyTree> device_property_tree_;
  // point selection
  std::vector<PointSelectionCallback> point_selection_funcs_;
  // refresh state
  bool refresh_{false};
  // play state
  PlayerState player_state_;
  std::vector<TriggerCallback> start_player_funcs_;
  std::vector<TriggerCallback> pause_player_funcs_;
  std::vector<TriggerCallback> start_recorder_funcs_;
  std::vector<TriggerCallback> stop_recorder_funcs_;
  // device name
  std::string device_name_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_DEVICE_CONTEXT_H
