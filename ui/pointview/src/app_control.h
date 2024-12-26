//
// Created by tongdayang on 2/2/23.
//

#ifndef POINTVIEW_APP_CONTROL_H
#define POINTVIEW_APP_CONTROL_H

// std
#include <map>
#include <memory>
// pcl
#include "utils/visualization/pcl_visualizer.h"
// Qt
#include <QMessageBox>
#include <QObject>
// custom
#include "device_factory.h"
#include "utils/common/device_base.h"

namespace autox {
namespace pointview {

class AppControl : public QObject {
  Q_OBJECT
 public:
  enum class CameraPose { Front, Top, Init, Left, Right };

 public:
  AppControl();
  ~AppControl();
  void setCameraPose(CameraPose pose);
  int addDevice(std::string device_type,
                std::shared_ptr<Config> config = nullptr,
                const std::string& device_name = "unknown");
  void Init(std::shared_ptr<DisplayContext> display_context);
  QStringList getDeviceTypes();
  int isValidTreeItem(QTreeWidgetItem* item);
  bool removeDevice(int device_id);
  bool setCurrentPlayer(int device_id);
  bool readyToSelect();
  bool readyToUpdate();
  int getCurrentPlayerProgress();
  bool getRecordState();
  std::string getRecordPath();
  void setCurrentPlayerProgress(int value);
  bool playingStateChange();
  bool recordStateChange();
  std::shared_ptr<PlayerState> getCurrentPlayerState();
  bool resetPlayerProgress();
  void save2Config(std::shared_ptr<Config> config);
  void setInitCameraPosition(std::vector<float>& view_point);

 signals:
  void SyncPlayerState();

 protected slots:
  void PlayerStateChange();

 protected:
  // point pick
  void selectPointCloudCallback(
      const pcl::visualization::AreaPickingEvent& event, void* userData);

 private:
  // device
  int unique_device_id_{1};
  std::vector<std::shared_ptr<DeviceBase>> device_list_;
  std::shared_ptr<DeviceFactory> device_factory_;
  QStringList device_types_;
  // display context
  std::shared_ptr<DisplayContext> display_context_;
  // player parameters
  std::shared_ptr<DeviceBase> current_player_device_;
  //
  std::map<int, std::string> device_type_map_;
  // init camera position
  std::vector<float> init_camera_position_;
  // sum of active device
  int active_device_{0};
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_APP_CONTROL_H
