
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "lidar_intrinsics.h"

#include <QMessageBox>
#include <filesystem>
#include <iostream>

#include "utils.h"

namespace autox {
namespace pointview {

LidarIntrinsics::LidarIntrinsics(std::shared_ptr<DeviceContext> device_context,
                                 int laser_num)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  laser_num_ = laser_num;
  elevation_offset_.assign(laser_num, 0);
  azimuth_offset_.assign(laser_num, 0);
  distance_offset_.assign(laser_num, 0);
  origin_x_offset_.assign(laser_num_, 0);
  origin_y_offset_.assign(laser_num_, 0);
  origin_z_offset_.assign(laser_num_, 0);
  // lidar intrinsics load&save
  auto sub = device_context_->getPropertyTree()->createPropertySubTree(
      "lidar intrinsics");

  label_intrinsics_file_ = std::make_shared<QLabel>();
  label_intrinsics_file_->setText("no intrinsics file");
  label_intrinsics_file_->setAlignment(Qt::AlignRight);
  sub->addProperty("file path", label_intrinsics_file_);
  // load
  load_intrinsics_button_ = std::make_shared<QPushButton>();
  load_intrinsics_button_->setText("load");
  sub->addProperty("load file", load_intrinsics_button_);
  connect(load_intrinsics_button_.get(), &QPushButton::clicked, [this]() {
    if (device_context_->getPlayerState().is_playing) {
      LOG(INFO) << "Don't load intrinsics file when playing.";
      return;
    }
    QString qfilename =
        GetOpenFileName("Load lidar intrinsics file", QDir::homePath(),
                        "CSV Files(*.csv);;All Files(*.*)");
    if (qfilename.isNull()) {
      LOG(INFO) << "Do not select a target directory.";
      return;
    }
    loadIntrinsics(qfilename.toStdString());
  });
  // save
  save_intrinsics_button_ = std::make_shared<QPushButton>();
  save_intrinsics_button_->setText("save");
  sub->addProperty("save file", save_intrinsics_button_);
  connect(save_intrinsics_button_.get(), &QPushButton::clicked, [this]() {
    QString qfilename = GetSaveFileName("Save lidar intrinsics file",
                                        QDir::homePath() + "/Untitled.csv",
                                        "CSV Files(*.csv);;All Files(*.*)");
    if (qfilename.isNull()) {
      LOG(INFO) << "Do not select a target file.";
      return;
    }
    saveIntrinsics(qfilename.toStdString());
  });
  // auto load
  checkbox_auto_load_ = std::make_shared<QCheckBox>();
  sub->addProperty("auto load", checkbox_auto_load_);
  // for manual calibration
  auto calib_sub = sub->createPropertySubTree("manual calibration");
  checkbox_enable_calib_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("enable", checkbox_enable_calib_);
  connect(checkbox_enable_calib_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_calib_ = state > 0;
            if (enable_calib_) {
              getIntrinsics();
              device_context_->refreshPointCloud();
            }
            updateCalibWidgetValue();
          });
  // checkbox_enable_calib_elevation
  checkbox_enable_calib_elevation_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("enable elevation", checkbox_enable_calib_elevation_);
  connect(checkbox_enable_calib_elevation_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_calib_elevation_ = state > 0;
            updateCalibWidgetValue();
          });
  // checkbox_enable_calib_azimuth
  checkbox_enable_calib_azimuth_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("enable azimuth", checkbox_enable_calib_azimuth_);
  connect(checkbox_enable_calib_azimuth_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_calib_azimuth_ = state > 0;
            updateCalibWidgetValue();
          });
  // checkbox_enable_calib_distance
  checkbox_enable_calib_distance_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("enable distance", checkbox_enable_calib_distance_);
  connect(checkbox_enable_calib_distance_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_calib_distance_ = state > 0;
            updateCalibWidgetValue();
          });
  // checkbox_enable_calib_origin_
  checkbox_enable_calib_origin_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("enable origin", checkbox_enable_calib_origin_);
  connect(checkbox_enable_calib_origin_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            enable_calib_origin_ = state > 0;
            updateCalibWidgetValue();
          });
  // checkbox_refine_mode_
  checkbox_refine_mode_ = std::make_shared<QCheckBox>();
  calib_sub->addProperty("refine mode", checkbox_refine_mode_);
  connect(checkbox_refine_mode_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            bool enable = state > 0;
            if (enable) {
              spinbox_elevation_offset_->setSingleStep(angle_step_ * 0.1);
              spinbox_azimuth_offset_->setSingleStep(angle_step_ * 0.1);
              spinbox_distance_offset_->setSingleStep(distance_step_ * 0.1);
              spinbox_origin_x_offset_->setSingleStep(distance_step_ * 0.1);
              spinbox_origin_y_offset_->setSingleStep(distance_step_ * 0.1);
              spinbox_origin_z_offset_->setSingleStep(distance_step_ * 0.1);
            } else {
              spinbox_elevation_offset_->setSingleStep(angle_step_);
              spinbox_azimuth_offset_->setSingleStep(angle_step_);
              spinbox_distance_offset_->setSingleStep(distance_step_);
              spinbox_origin_x_offset_->setSingleStep(distance_step_);
              spinbox_origin_y_offset_->setSingleStep(distance_step_);
              spinbox_origin_z_offset_->setSingleStep(distance_step_);
            }
          });
  // laser id
  spinbox_laser_id_ = std::make_shared<QSpinBox>();
  spinbox_laser_id_->setRange(1, laser_num);
  spinbox_laser_id_->setValue(1);
  calib_sub->addProperty("laser id", spinbox_laser_id_);
  connect(spinbox_laser_id_.get(),
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          [this](int v) {
            current_laser_id_ = v - 1;
            updateCalibWidgetValue();
            device_context_->refreshPointCloud();
          });
  // spinbox_elevation_offset
  spinbox_elevation_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_elevation_offset_->setRange(-180, 180);
  spinbox_elevation_offset_->setSingleStep(angle_step_);
  spinbox_elevation_offset_->setSuffix("deg");
  calib_sub->addProperty("elevation offset", spinbox_elevation_offset_);
  connect(spinbox_elevation_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            elevation_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  // spinbox_azimuth_offset
  spinbox_azimuth_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_azimuth_offset_->setRange(-180, 180);
  spinbox_azimuth_offset_->setSingleStep(angle_step_);
  spinbox_azimuth_offset_->setSuffix("deg");
  calib_sub->addProperty("azimuth offset", spinbox_azimuth_offset_);
  connect(spinbox_azimuth_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            azimuth_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  // spinbox_distance_offset
  spinbox_distance_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_distance_offset_->setRange(-10, 10);
  spinbox_distance_offset_->setSingleStep(distance_step_);
  spinbox_distance_offset_->setDecimals(3);
  spinbox_distance_offset_->setSuffix("m");
  calib_sub->addProperty("distance offset", spinbox_distance_offset_);
  connect(spinbox_distance_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            distance_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  // spinbox_origin_x_offset
  spinbox_origin_x_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_origin_x_offset_->setRange(-10, 10);
  spinbox_origin_x_offset_->setSingleStep(distance_step_);
  spinbox_origin_x_offset_->setDecimals(3);
  spinbox_origin_x_offset_->setSuffix("m");
  calib_sub->addProperty("origin x", spinbox_origin_x_offset_);
  connect(spinbox_origin_x_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            origin_x_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  // spinbox_origin_y_offset
  spinbox_origin_y_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_origin_y_offset_->setRange(-10, 10);
  spinbox_origin_y_offset_->setSingleStep(distance_step_);
  spinbox_origin_y_offset_->setDecimals(3);
  spinbox_origin_y_offset_->setSuffix("m");
  calib_sub->addProperty("origin y", spinbox_origin_y_offset_);
  connect(spinbox_origin_y_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            origin_y_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  // spinbox_origin_z_offset
  spinbox_origin_z_offset_ = std::make_shared<QDoubleSpinBox>();
  spinbox_origin_z_offset_->setRange(-10, 10);
  spinbox_origin_z_offset_->setSingleStep(distance_step_);
  spinbox_origin_z_offset_->setDecimals(3);
  spinbox_origin_z_offset_->setSuffix("m");
  calib_sub->addProperty("origin z", spinbox_origin_z_offset_);
  connect(spinbox_origin_z_offset_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            origin_z_offset_[current_laser_id_] = v;
            setIntrinsics();
            device_context_->refreshPointCloud();
          });
  updateCalibWidgetValue();
}

bool LidarIntrinsics::setIntrinsics() {
  bool success = false;
  if (set_intrinsics_cb_) {
    success = set_intrinsics_cb_(elevation_offset_, azimuth_offset_,
                                 distance_offset_);
  }
  if (set_intrinsics_cb2_) {
    success = set_intrinsics_cb2_(elevation_offset_, azimuth_offset_,
                                  distance_offset_, origin_x_offset_,
                                  origin_y_offset_, origin_z_offset_);
  }
  return success;
}

bool LidarIntrinsics::getIntrinsics() {
  bool success = false;
  if (get_intrinsics_cb_) {
    success = get_intrinsics_cb_(elevation_offset_, azimuth_offset_,
                                 distance_offset_);
  }
  if (get_intrinsics_cb2_) {
    success = get_intrinsics_cb2_(elevation_offset_, azimuth_offset_,
                                  distance_offset_, origin_x_offset_,
                                  origin_y_offset_, origin_z_offset_);
  }
  // check valid
  if (elevation_offset_.size() != laser_num_ ||
      azimuth_offset_.size() != laser_num_ ||
      distance_offset_.size() != laser_num_ ||
      origin_x_offset_.size() != laser_num_ ||
      origin_y_offset_.size() != laser_num_ ||
      origin_z_offset_.size() != laser_num_) {
    elevation_offset_.resize(laser_num_, 0);
    azimuth_offset_.resize(laser_num_, 0);
    distance_offset_.resize(laser_num_, 0);
    origin_x_offset_.resize(laser_num_, 0);
    origin_y_offset_.resize(laser_num_, 0);
    origin_z_offset_.resize(laser_num_, 0);
    LOG(INFO) << "invalid size of intrinsics data.";
    success = false;
  }
  return success;
}

void LidarIntrinsics::loadIntrinsics(const std::string& filename) {
  if (device_context_->getPlayerState().is_playing) {
    return;
  }
  if (!set_intrinsics_cb_ && !set_intrinsics_cb2_) {
    return;
  }
  load_filepath_ = filename;
  label_intrinsics_file_->setText(QString::fromStdString(filename));
  // init data
  int laser_id;
  // open file
  std::string line;
  std::ifstream fp(filename);
  getline(fp, line);
  // check csv valid
  std::string header1 = "Channel,Elevation,Azimuth";
  std::string header2 = "Channel,Elevation,Azimuth,Distance";
  std::string header3 =
      "Channel,Elevation,Azimuth,Distance,Origin X,Origin Y,Origin Z";
  if ((line.substr(0, header1.size()) != header1) &&
      (line.substr(0, header2.size()) != header2) &&
      (line.substr(0, header3.size()) != header3)) {
    LOG(INFO) << "invalid intrainsic file: " << filename
              << ", header: " << line;
    return;
  }
  // get data
  for (size_t i = 0; i < laser_num_; i++) {
    getline(fp, line);
    sscanf(line.c_str(), "%d,%lf,%lf,%lf,%lf,%lf,%lf", &laser_id,
           &(elevation_offset_[i]), &(azimuth_offset_[i]),
           &(distance_offset_[i]), &(origin_x_offset_[i]),
           &(origin_y_offset_[i]), &(origin_z_offset_[i]));
  }
  // set intrinsics
  if (setIntrinsics()) {
    LOG(INFO) << "load intrinsics successfully.";
    getIntrinsics();
    // need update calibration widget
    updateCalibWidgetValue();
  }
}

void LidarIntrinsics::saveIntrinsics(const std::string& filename) {
  if (!getIntrinsics()) {
    return;
  }
  // save
  std::ofstream out;
  out.open(filename, std::ios::out);
  out << "Channel,Elevation,Azimuth,Distance,Origin X,Origin Y,Origin Z";
  for (size_t i = 0; i < laser_num_; i++) {
    out << i + 1 << "," << elevation_offset_[i] << "," << azimuth_offset_[i]
        << "," << distance_offset_[i] << "," << origin_x_offset_[i] << ","
        << origin_y_offset_[i] << "," << origin_z_offset_[i];
  }
  LOG(INFO) << "save intrinsics successfully.";
}

void LidarIntrinsics::updateCalibWidgetValue() {
  if (enable_calib_) {
    checkbox_enable_calib_elevation_->setEnabled(true);
    checkbox_enable_calib_azimuth_->setEnabled(true);
    checkbox_enable_calib_distance_->setEnabled(true);
    checkbox_enable_calib_origin_->setEnabled(true);
    checkbox_refine_mode_->setEnabled(true);
    spinbox_laser_id_->setEnabled(true);
    spinbox_elevation_offset_->setEnabled(enable_calib_elevation_);
    spinbox_azimuth_offset_->setEnabled(enable_calib_azimuth_);
    spinbox_distance_offset_->setEnabled(enable_calib_distance_);
    spinbox_origin_x_offset_->setEnabled(enable_calib_origin_);
    spinbox_origin_y_offset_->setEnabled(enable_calib_origin_);
    spinbox_origin_z_offset_->setEnabled(enable_calib_origin_);
    // update value
    spinbox_elevation_offset_->setValue(elevation_offset_[current_laser_id_]);
    spinbox_azimuth_offset_->setValue(azimuth_offset_[current_laser_id_]);
    spinbox_distance_offset_->setValue(distance_offset_[current_laser_id_]);
    spinbox_origin_x_offset_->setValue(origin_x_offset_[current_laser_id_]);
    spinbox_origin_y_offset_->setValue(origin_y_offset_[current_laser_id_]);
    spinbox_origin_z_offset_->setValue(origin_z_offset_[current_laser_id_]);
  } else {
    checkbox_enable_calib_elevation_->setEnabled(false);
    checkbox_enable_calib_azimuth_->setEnabled(false);
    checkbox_enable_calib_distance_->setEnabled(false);
    checkbox_enable_calib_origin_->setEnabled(false);
    checkbox_refine_mode_->setEnabled(false);
    spinbox_laser_id_->setEnabled(false);
    spinbox_elevation_offset_->setEnabled(false);
    spinbox_azimuth_offset_->setEnabled(false);
    spinbox_distance_offset_->setEnabled(false);
    spinbox_origin_x_offset_->setEnabled(false);
    spinbox_origin_y_offset_->setEnabled(false);
    spinbox_origin_z_offset_->setEnabled(false);
  }
}

void LidarIntrinsics::registerSetIntrinsicsCallback(SetIntrinsicsCallback cb) {
  set_intrinsics_cb_ = cb;
}

void LidarIntrinsics::registerSetIntrinsicsCallback(SetIntrinsicsCallback2 cb) {
  set_intrinsics_cb2_ = cb;
}

void LidarIntrinsics::registerGetIntrinsicsCallback(GetIntrinsicsCallback cb) {
  get_intrinsics_cb_ = cb;
}

void LidarIntrinsics::registerGetIntrinsicsCallback(GetIntrinsicsCallback2 cb) {
  get_intrinsics_cb2_ = cb;
}

void LidarIntrinsics::registerGetLaserIdCallback(GetLaserIdCallback cb) {
  get_laser_id_cb_ = cb;
}

bool LidarIntrinsics::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  bool auto_load = false;
  config->getParameter(namespace_ + ".auto_load", auto_load);
  config->getParameter(namespace_ + ".filepath", load_filepath_);
  checkbox_auto_load_->setChecked(auto_load);
  if (auto_load && std::filesystem::exists(load_filepath_)) {
    loadIntrinsics(load_filepath_);
  }
  return true;
}

bool LidarIntrinsics::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter(namespace_ + ".auto_load",
                       checkbox_auto_load_->isChecked());
  config->setParameter(namespace_ + ".filepath", load_filepath_);
  return true;
}

void LidarIntrinsics::update(PointCloudT::Ptr cloud) {
  if (!enable_calib_ || !get_laser_id_cb_) {
    return;
  }
  for (size_t i = 0; i < cloud->points.size(); i++) {
    auto& p = cloud->points[i];
    if (p.a == 0) {
      continue;
    }
    if (get_laser_id_cb_(i) != current_laser_id_) {
      continue;
    }
    // set color
    p.r = 255;
    p.g = 0;
    p.b = 0;
  }
}

}  // namespace pointview
}  // namespace autox
