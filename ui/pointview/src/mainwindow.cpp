#include "mainwindow.h"

#include <vtkGenericOpenGLRenderWindow.h>

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QKeyEvent>
#include <QProgressDialog>
#include <iostream>

#include "pcl/io/pcd_io.h"
#include "ui_mainwindow.h"
#include "utils/utils.h"

#if VTK_MAJOR_VERSION > 8
#include <vtkGenericOpenGLRenderWindow.h>
#endif
namespace autox::pointview {
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      delete_action_(new QAction(this)),
      update_timer_(new QTimer(this)),
      record_timer_(new QTimer(this)),
      app_control_(new AppControl()) {
  ui_->setupUi(this);
  delete_action_->setText("Delete");
#if VTK_MAJOR_VERSION > 8
  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  auto renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  viewer_.reset(new autox::visualization::PCLVisualizer(renderer, renderWindow,
                                                        "viewer", false));
  ui_->PCLViewer->setRenderWindow(viewer_->getRenderWindow());
  viewer_->setupInteractor(ui_->PCLViewer->interactor(),
                           ui_->PCLViewer->renderWindow());
#else
  viewer_.reset(new autox::visualization::PCLVisualizer("viewer", false));
  ui_->PCLViewer->SetRenderWindow(viewer_->getRenderWindow());
  viewer_->setupInteractor(ui_->PCLViewer->GetInteractor(),
                           ui_->PCLViewer->GetRenderWindow());
#endif
  //  initFromConfig value
  //  set play device index
  on_DeviceComboBox_currentIndexChanged(-1);
  // initialize display context
  ui_->splitter->setStretchFactor(0, 8);
  ui_->splitter->setStretchFactor(1, 1);
  ui_->PointCloudTree->header()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui_->ToolTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  display_context_ = std::make_shared<DisplayContext>(
      this, viewer_, ui_->PointCloudTree, ui_->ToolTree);
  // set top widget font
  std::shared_ptr<QFont> font = std::make_shared<QFont>();
  font->setPointSize(13);
  display_context_->setFont(font);
  // init app control
  app_control_->Init(display_context_);
  // tools: coordinate axis, grid
  coordinate_axis_ = std::make_shared<CoordinateAxis>(display_context_);
  addNewTool(coordinate_axis_);
  grid_ = std::make_shared<Grid>(display_context_);
  addNewTool(grid_);
  scale_bar_ = std::make_shared<ScaleBar>(display_context_);
  addNewTool(scale_bar_);
  circle_ = std::make_shared<Circle>(display_context_);
  addNewTool(circle_);
  // new tools: point pick
  point_distance_measure_ =
      std::make_shared<PointDistanceMeasure>(display_context_);
  // get desktop size
  this->move(QApplication::desktop()->rect().center() - this->rect().center());
  // reset slider
  setSliderMaxValue(0);
  // connect ui signals
  connect(update_timer_.get(), SIGNAL(timeout()), this,
          SLOT(on_UpdatePointCloud()));
  connect(record_timer_.get(), SIGNAL(timeout()), this,
          SLOT(on_UpdateRecordTimer()));
  connect(ui_->PointCloudTree, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this,
          SLOT(on_propertyTreeItemClicked(QTreeWidgetItem*, int)));
  connect(delete_action_.get(), SIGNAL(triggered(bool)), this,
          SLOT(on_deleteActionClicked(bool)));
  connect(ui_->AddSourceAction, SIGNAL(triggered(bool)), this,
          SLOT(on_addSourceClicked()));
  connect(ui_->AddSourceButton, SIGNAL(clicked(bool)), this,
          SLOT(on_addSourceClicked()));
  connect(ui_->GridAction, SIGNAL(triggered(bool)), this,
          SLOT(on_toolActionTriggered()));
  connect(ui_->FrameAction, SIGNAL(triggered(bool)), this,
          SLOT(on_toolActionTriggered()));
  connect(app_control_.get(), SIGNAL(SyncPlayerState()), this,
          SLOT(on_SyncPlayer()));
  connect(ui_->DeleteButton, SIGNAL(clicked(bool)), this,
          SLOT(on_deleteActionClicked(bool)));
  // reset camera position
  on_ResetViewAction_triggered();
  // start timer
  update_timer_->start(5);
  record_timer_->start(5);
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::refreshView() {
#if VTK_MAJOR_VERSION > 8
  ui_->PCLViewer->renderWindow()->Render();
#else
  ui_->PCLViewer->update();
#endif
}

void MainWindow::on_addSourceClicked() {
  auto device_types_ = app_control_->getDeviceTypes();
  bool ok;
  // get device type
  QString selected_type =
      QInputDialog::getItem(this, tr("Select device type"), tr("Device type:"),
                            device_types_, 0, false, &ok);
  if (!ok || selected_type.isEmpty()) {
    return;
  }
  QString device_name =
      QInputDialog::getText(this, tr("Input device name"), tr("Device name:"),
                            QLineEdit::Normal, selected_type, &ok);
  if (!ok || device_name.isEmpty()) {
    return;
  }

  int device_id = app_control_->addDevice(selected_type.toStdString(), nullptr,
                                          device_name.toStdString());

  if (device_id > -1) {
    ui_->DeviceComboBox->addItem(QString::number(device_id));
  }
}

void MainWindow::on_UpdateRecordTimer() {
  static long long index = 0;
  static bool reset = true;
  static auto last = std::chrono::steady_clock::now();
  if (app_control_->getRecordState()) {
    if (reset) {
      last = std::chrono::steady_clock::now();
      reset = false;
      index = 0;
      ui_->textEdit->setText(QString::number(index) + "s");
    }
    std::chrono::duration<double> elapsed;
    auto current = std::chrono::steady_clock::now();
    elapsed = current - last;
    if (elapsed.count() >= 1.0) {
      last = current;
      if (!app_control_->getCurrentPlayerState()->record_wait)
        ui_->textEdit->setText(QString::number(++index) + "s");
    }
    ui_->textEdit_2->setText(QString::number(
        app_control_->getCurrentPlayerState()->current_frame_idx));
  } else {
    reset = true;
  }
}
void MainWindow::on_UpdatePointCloud() {
  if (app_control_->readyToUpdate()) {
    // update player if it's connecting to device
    if (!slider_press_) {
      int current_idx = app_control_->getCurrentPlayerProgress();
      if (current_idx > -1) {
        setSliderCurrentValue(current_idx);
      }
    }
    refreshView();
  }
}

void MainWindow::on_propertyTreeItemClicked(QTreeWidgetItem* item, int column) {
  if (qApp->mouseButtons() == Qt::RightButton) {
    select_device_id_ = app_control_->isValidTreeItem(item);
    if (select_device_id_ > -1) {
      std::shared_ptr<QMenu> qMenu(new QMenu(ui_->PointCloudTree));
      qMenu->addAction(delete_action_.get());
      qMenu->exec(QCursor::pos());
      return;
    }
  }
  if (qApp->mouseButtons() == Qt::LeftButton) {
    select_device_id_ = app_control_->isValidTreeItem(item);
  }
}

void MainWindow::on_deleteActionClicked(bool checked) {
  QMessageBox msgBox;
  msgBox.setText("The device will be deleted.");
  msgBox.setInformativeText("Do you want to delete it?");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Ok && select_device_id_ >= 0) {
    // reset play combobox
    ui_->DeviceComboBox->setCurrentIndex(-1);
    int index =
        ui_->DeviceComboBox->findText(QString::number(select_device_id_));
    if (index != -1) {
      ui_->DeviceComboBox->removeItem(index);
    }
    app_control_->removeDevice(select_device_id_);
    // update global state
    select_device_id_ = -1;
    refreshView();
  }
}

void MainWindow::addNewTool(const std::shared_ptr<ToolBase>& tool_ptr) {
  connect(tool_ptr.get(), SIGNAL(refreshView()), this, SLOT(refreshView()));
}

void MainWindow::on_FrontViewAction_triggered() {
  app_control_->setCameraPose(AppControl::CameraPose::Front);
}

void MainWindow::on_TopViewAction_triggered() {
  app_control_->setCameraPose(AppControl::CameraPose::Top);
}

void MainWindow::on_ResetViewAction_triggered() {
  app_control_->setCameraPose(AppControl::CameraPose::Init);
}

void MainWindow::on_PlayButton_clicked() {
  bool current_playing_state = app_control_->playingStateChange();
  setPlayButtonState(current_playing_state);
}

void MainWindow::on_DeviceComboBox_currentIndexChanged(int index) {
  if (index > -1) {
    int device_id = ui_->DeviceComboBox->currentText().toInt();
    app_control_->setCurrentPlayer(device_id);
  }

  on_SyncPlayer();
}

void MainWindow::on_SyncPlayer() {
  auto device_play_state = app_control_->getCurrentPlayerState();
  static long index = 0;
  // slider reset
  if (device_play_state) {
    // set play button
    setPlayButtonState(device_play_state->is_playing);
    setRecordButtonState(device_play_state->is_recording);
    int frame_number = 0;
    int current_frame_number = 0;
    if (device_play_state->is_playback) {
      // enable play back widget
      ui_->PlayBackWidget->setEnabled(true);
      // disable record widget
      ui_->RecordButton->setEnabled(false);
      frame_number = device_play_state->total_frames - 1;
      current_frame_number = device_play_state->current_frame_idx;
    } else {
      // disable play back widget
      ui_->PlayBackWidget->setEnabled(false);
      // enable record widget
      ui_->RecordButton->setEnabled(true);
    }
    setSliderMaxValue(frame_number);
    setSliderCurrentValue(current_frame_number);
  } else {
    ui_->PlayBackWidget->setEnabled(false);
    ui_->RecordButton->setEnabled(false);
  }
}

void MainWindow::on_ResetButton_clicked() {
  app_control_->resetPlayerProgress();
  setPlayButtonState(false);
  setSliderCurrentValue(0);
}
void MainWindow::on_PlaySlider_sliderReleased() {
  if (slider_press_) {
    int pos = ui_->PlaySlider->value();
    app_control_->setCurrentPlayerProgress(pos);
    setSliderCurrentValue(pos);

    auto state = app_control_->getCurrentPlayerState();
    if (state->need_playback_jumb && !state->is_playing) {
      using namespace std::chrono_literals;
      QProgressDialog progress("Jump to the specified framex, in progress...",
                               "Abort", 0, 100, nullptr);
      progress.setWindowModality(Qt::WindowModal);
      progress.show();
      progress.setValue(0);
      while (1) {
        auto state = app_control_->getCurrentPlayerState();
        if (!state->need_playback_jumb) break;
        double rate = state->rate_playback_jump;
        progress.setValue((int)(rate * 100));
        std::this_thread::sleep_for(50ms);
      }
      progress.setValue(100);
    }
  }
  slider_press_ = false;
}

void MainWindow::on_PrevButton_clicked() { movePlayFrame(-1); }

void MainWindow::on_NextButton_clicked() { movePlayFrame(1); }

void MainWindow::on_PlaySlider_sliderPressed() { slider_press_ = true; }

void MainWindow::on_CurrentFrameSpinBox_editingFinished() {
  int pos = ui_->CurrentFrameSpinBox->value();
  app_control_->setCurrentPlayerProgress(pos);
  setSliderCurrentValue(pos);
}

void MainWindow::setPlayButtonState(bool state) {
  if (state) {
    ui_->PlayButton->setStyleSheet(
        "QPushButton{border-image: url(:/pointview_icon/play-pause.png);}");
  } else {
    ui_->PlayButton->setStyleSheet(
        "QPushButton{border-image: "
        "url(:/pointview_icon/play-start-disable.png);}\n"
        "QPushButton:hover{border-image: "
        "url(:/pointview_icon/play-start.png);}");
  }
  resetSelectionActionState();
}

void MainWindow::movePlayFrame(int step) {
  int pos = app_control_->getCurrentPlayerProgress();
  if (pos > -1) {
    app_control_->setCurrentPlayerProgress(pos + step);
    auto state = app_control_->getCurrentPlayerState();
    if (state->need_playback_jumb && !state->is_playing) {
      using namespace std::chrono_literals;
      QProgressDialog progress("Jump to the specified framex, in progress...",
                               "Abort", 0, 100, nullptr);
      progress.setWindowModality(Qt::WindowModal);
      progress.show();
      progress.setValue(0);
      while (1) {
        auto state = app_control_->getCurrentPlayerState();
        if (!state->need_playback_jumb) break;
        double rate = state->rate_playback_jump;
        progress.setValue((int)(rate * 100));
        std::this_thread::sleep_for(50ms);
      }
      progress.setValue(100);
    }
  }
}

void MainWindow::on_RecordButton_clicked() {
  if (app_control_->getRecordPath().empty()) {
    QMessageBox::warning(0, "提示", "please select record path!",
                         QMessageBox::Ok | QMessageBox::Default, 0);
    return;
  }
  bool current_record_state = app_control_->recordStateChange();
  setRecordButtonState(current_record_state);
}

void MainWindow::setRecordButtonState(bool state) {
  if (state) {
    ui_->RecordButton->setStyleSheet(
        "QPushButton{border-image: url(:/pointview_icon/record-off.png);}");
  } else {
    ui_->RecordButton->setStyleSheet(
        "QPushButton{border-image: "
        "url(:/pointview_icon/record-on-disable.png);}\n"
        "QPushButton:hover{border-image: "
        "url(:/pointview_icon/record-on.png);}");
  }
}

void MainWindow::on_SelectAction_triggered(bool checked) {
  if (checked != select_state_) {
    select_state_ = checked;
    ui_->PCLViewer->setFocus();
    QKeyEvent* key =
        new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_X, Qt::NoModifier, "x");
    QApplication::sendEvent(ui_->PCLViewer, key);
  }
  if (select_state_ != ui_->SelectAction->isChecked()) {
    ui_->SelectAction->setChecked(select_state_);
  }
}

void MainWindow::on_OpenConfigAction_triggered() {
  std::string config_file_path =
      GetOpenFileName("Open Config File", QDir::homePath(),
                      "YAML Files(*.yaml *.yml);;All Files(*.*)")
          .toStdString();

  if (config_file_path != "") {
    // open config file
    std::cout << "Open " << config_file_path << std::endl;
    std::shared_ptr<Config> config(new Config);
    config->openConfig(config_file_path);
    // init lidar
    auto lidar_lists = config->getSubConfig("lidar_lists");
    for (auto it = lidar_lists->begin(); it != lidar_lists->end(); it++) {
      int device_id = -1;
      auto lidar_config =
          lidar_lists->getSubConfig(it->first.as<std::string>());
      std::string device_type;
      lidar_config->getParameter("device_type", device_type);
      std::string device_name = device_type;
      lidar_config->getParameter("device_name", device_name);
      device_id =
          app_control_->addDevice(device_type, lidar_config, device_name);
      if (device_id > -1) {
        ui_->DeviceComboBox->addItem(QString::number(device_id));
      }
    }
    // init view point
    std::vector<float> view_point;
    config->getParameter("view_point", view_point);
    app_control_->setInitCameraPosition(view_point);
    app_control_->setCameraPose(AppControl::CameraPose::Init);
  } else {
    std::cout << "do not select config file." << std::endl;
  }
}

void MainWindow::on_toolActionTriggered() {
  ui_->tabWidget->setCurrentIndex(1);
}

void MainWindow::resetSelectionActionState() {
  bool ready = app_control_->readyToSelect();
  if (ready) {
    ui_->SelectAction->setEnabled(true);
  } else {
    on_SelectAction_triggered(false);
    ui_->SelectAction->setEnabled(false);
  }
}

bool MainWindow::setSliderMaxValue(int max_value) {
  if (max_value > 0) {
    ui_->PlaySlider->setMaximum(max_value);
    ui_->TotalFrameLabel->setNum(max_value);
    ui_->CurrentFrameSpinBox->setMaximum(max_value);
    return true;
  }
  return false;
}

bool MainWindow::setSliderCurrentValue(int crt_value) {
  int value = ui_->PlaySlider->value();
  if (value != crt_value) {
    ui_->PlaySlider->setValue(crt_value);
  }
  value = ui_->CurrentFrameSpinBox->value();
  if (value != crt_value) {
    ui_->CurrentFrameSpinBox->setValue(crt_value);
  }
  return true;
}

void MainWindow::on_SaveConfigAction_triggered() {
  auto conf = std::make_shared<Config>();
  app_control_->save2Config(conf);
  QString fileName =
      GetSaveFileName("Save config file", QDir::homePath() + "/Untitled.yaml",
                      "YAML Files(*.yaml *.yml);;All Files(*.*)");
  if (fileName.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  QMessageBox msgBox;
  msgBox.setStyleSheet("QLabel{min-width: 200px;}");
  if (conf->saveConfig(fileName.toStdString())) {
    msgBox.setText("Success.");
    msgBox.setInformativeText("Save config in " + fileName + " successfully.");
  } else {
    msgBox.setText("Error.");
    msgBox.setInformativeText("Failed to save config in " + fileName + ".");
  }

  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

void MainWindow::on_LeftViewAction_triggered() {
  app_control_->setCameraPose(AppControl::CameraPose::Left);
}

void MainWindow::on_RightViewAction_triggered() {
  app_control_->setCameraPose(AppControl::CameraPose::Right);
}

void MainWindow::on_HelpAction_triggered() {
  QMessageBox msgBox;
  msgBox.setBaseSize(400, 200);
  msgBox.setWindowTitle("Help");
  QString help_msg =
      "XLidar\n default port: 2370, default packet number(every minute): "
      "200000\n\n"
      "Blidar\n default port: 2469, default packet number(every minute): "
      "9000\n";
  msgBox.setInformativeText(help_msg);
  msgBox.setStandardButtons(QMessageBox::Help | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Help) {
    QString url = "https://wiki.autox.clu/collection/pointview-DsFSYKvdYD";
    QDesktopServices::openUrl(QUrl(url.toLatin1()));
  }
}

void MainWindow::on_DemoModeAction_triggered(bool checked) {
  if (checked != demo_mode_) {
    demo_mode_ = checked;
    if (demo_mode_) {
      ui_->PlayerWidget->hide();
      ui_->toolBar->hide();
      ui_->tabWidget->hide();
    } else {
      ui_->PlayerWidget->show();
      ui_->toolBar->show();
      ui_->tabWidget->show();
    }
  }
  if (demo_mode_ != ui_->DemoModeAction->isChecked()) {
    ui_->DemoModeAction->setChecked(demo_mode_);
  }
}

}  // namespace autox::pointview
