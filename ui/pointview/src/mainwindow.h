#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QTreeWidget>
// std
#include <memory>
#include <thread>
#include <vector>
// pcl
#include "utils/visualization/pcl_visualizer.h"
// vtk
#include <vtkAreaPicker.h>
// custom
#include "app_control.h"
#include "device_factory.h"
#include "tools/circle.h"
#include "tools/coordinate_axis.h"
#include "tools/grid.h"
#include "tools/point_distance_measure.h"
#include "tools/scale_bar.h"
#include "ui_mainwindow.h"
#include "utils/config.h"

namespace Ui {
class MainWindow;
}

namespace autox {
namespace pointview {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);

  ~MainWindow();

 public slots:
  void on_addSourceClicked();
  void refreshView();
  void on_SyncPlayer();

 private slots:
  void on_UpdatePointCloud();
  void on_UpdateRecordTimer();
  void on_propertyTreeItemClicked(QTreeWidgetItem* item, int column);
  void on_deleteActionClicked(bool checked);
  void on_FrontViewAction_triggered();
  void on_TopViewAction_triggered();
  void on_LeftViewAction_triggered();
  void on_RightViewAction_triggered();
  void on_ResetViewAction_triggered();
  void on_PlayButton_clicked();
  void on_DeviceComboBox_currentIndexChanged(int index);
  void on_ResetButton_clicked();
  void on_PlaySlider_sliderReleased();
  void on_PrevButton_clicked();
  void on_NextButton_clicked();
  void on_PlaySlider_sliderPressed();
  void on_CurrentFrameSpinBox_editingFinished();
  void on_RecordButton_clicked();
  void on_SelectAction_triggered(bool checked);
  void on_OpenConfigAction_triggered();
  void on_toolActionTriggered();
  void on_SaveConfigAction_triggered();
  void on_HelpAction_triggered();
  void on_DemoModeAction_triggered(bool checked);

 private:
  void addNewTool(const std::shared_ptr<ToolBase>& tool_ptr);
  void setPlayButtonState(bool state);
  void movePlayFrame(int step);
  void setRecordButtonState(bool state);
  void resetSelectionActionState();
  // slider
  bool setSliderMaxValue(int max_value);
  bool setSliderCurrentValue(int crt_value);

 private:
  Ui::MainWindow* ui_;
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // property tree
  std::shared_ptr<QAction> delete_action_;
  int select_device_id_ = -1;
  // display context
  std::shared_ptr<DisplayContext> display_context_;
  // tool
  std::shared_ptr<CoordinateAxis> coordinate_axis_;
  std::shared_ptr<Grid> grid_;
  std::shared_ptr<PointDistanceMeasure> point_distance_measure_;
  std::shared_ptr<QTimer> update_timer_;
  std::shared_ptr<QTimer> record_timer_;
  bool slider_press_{false};
  // select point
  bool select_state_{false};
  // app control
  std::shared_ptr<AppControl> app_control_;
  // scale bar
  std::shared_ptr<ScaleBar> scale_bar_;
  // circle
  std::shared_ptr<Circle> circle_;
  // demo mode
  bool demo_mode_{false};
};
}  // namespace pointview
}  // namespace autox

#endif  // MAINWINDOW_H
