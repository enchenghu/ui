
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "utils/pose_setting.h"

#include <pcl/common/transforms.h>

namespace autox {
namespace pointview {

PoseSetting::PoseSetting(std::shared_ptr<DeviceContext> device_context)
    : QObject(device_context->getDisplayContext()->getParent()),
      device_context_(device_context) {
  auto sub = device_context_->getPropertyTree()->createPropertySubTree("pose");
  // checkbox_enable_
  checkbox_enable_ = std::make_shared<QCheckBox>();
  connect(checkbox_enable_.get(), &QCheckBox::stateChanged, [this](int state) {
    enable_ = state > 0;
    updatePoseSetting();
  });
  sub->addProperty("enable", checkbox_enable_);
  // spinbox_translations_
  std::vector<QString> translation_names = {"x", "y", "z"};
  for (size_t i = 0; i < translation_names.size(); i++) {
    auto spinbox = std::make_shared<QDoubleSpinBox>();
    spinbox->setSingleStep(0.01);
    spinbox->setDecimals(3);
    spinbox->setRange(-1000, 1000);
    spinbox->setValue(0);
    spinbox->setSuffix("m");
    connect(spinbox.get(),
            static_cast<void (QDoubleSpinBox::*)(double)>(
                &QDoubleSpinBox::valueChanged),
            [this](double /*v*/) { updatePoseSetting(); });
    spinbox_translations_.push_back(spinbox);
    sub->addProperty(translation_names[i], spinbox);
  }
  // spinbox_rotations_
  std::vector<QString> rotation_names = {
      "r11", "r12", "r13", "r21", "r22", "r23", "r31", "r32", "r33",
  };
  for (size_t i = 0; i < rotation_names.size(); i++) {
    auto spinbox = std::make_shared<QDoubleSpinBox>();
    spinbox->setSingleStep(0.000001);
    spinbox->setDecimals(6);
    spinbox->setRange(-1, 1);
    spinbox->setValue(pose_(i / 3, i % 3));
    spinbox->setEnabled(false);
    spinbox_rotations_.push_back(spinbox);
    sub->addProperty(rotation_names[i], spinbox);
  }
  // checkbox_use_euler_
  checkbox_use_euler_ = std::make_shared<QCheckBox>();
  connect(checkbox_use_euler_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            use_euler_angle_ = state > 0;
            updatePoseSetting();
          });
  sub->addProperty("use euler rpy", checkbox_use_euler_);
  // spinbox_euler_rpy_
  std::vector<QString> euler_rpy_names = {"roll", "pitch", "yaw"};
  for (size_t i = 0; i < euler_rpy_names.size(); i++) {
    auto spinbox = std::make_shared<QDoubleSpinBox>();
    spinbox->setSingleStep(0.1);
    spinbox->setDecimals(4);
    spinbox->setRange(-360, 360);
    spinbox->setValue(0);
    spinbox->setSuffix(" degree");
    spinbox->setEnabled(false);
    connect(spinbox.get(),
            static_cast<void (QDoubleSpinBox::*)(double)>(
                &QDoubleSpinBox::valueChanged),
            [this](double /*v*/) { updatePoseSetting(); });
    spinbox_euler_rpy_.push_back(spinbox);
    sub->addProperty(euler_rpy_names[i], spinbox);
  }
}

bool PoseSetting::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->getParameter(namespace_ + ".enable", enable_);
  checkbox_enable_->setChecked(enable_);
  // translation
  std::vector<double> translation;
  config->getParameter(namespace_ + ".translation", translation);
  if (translation.size() == 3) {
    for (int i = 0; i < 3; i++) {
      spinbox_translations_[i]->setValue(translation[i]);
    }
  }
  // rotation
  std::vector<double> rotation;
  config->getParameter(namespace_ + ".rotation", rotation);
  if (rotation.size() == 9) {
    for (int i = 0; i < 9; i++) {
      spinbox_rotations_[i]->setValue(rotation[i]);
    }
  }
  // euler_angle
  config->getParameter(namespace_ + ".use_euler_rpy", use_euler_angle_);
  checkbox_use_euler_->setChecked(use_euler_angle_);
  std::vector<double> rpy;
  config->getParameter(namespace_ + ".euler_rpy", rpy);
  if (rpy.size() == 3) {
    for (int i = 0; i < 3; i++) {
      spinbox_euler_rpy_[i]->setValue(rpy[i]);
    }
  }
  // update
  updatePoseSetting();
  return true;
}

bool PoseSetting::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  // enable
  config->setParameter(namespace_ + ".enable", enable_);
  // translation
  std::vector<double> translation;
  for (size_t i = 0; i < spinbox_translations_.size(); i++) {
    translation.push_back(spinbox_translations_[i]->value());
  }
  config->setParameter(namespace_ + ".translation", translation);
  // rotation
  std::vector<double> rotation;
  for (size_t i = 0; i < spinbox_rotations_.size(); i++) {
    rotation.push_back(spinbox_rotations_[i]->value());
  }
  config->setParameter(namespace_ + ".rotation", rotation);
  // euler_angle
  config->setParameter(namespace_ + ".use_euler_rpy", use_euler_angle_);
  std::vector<double> rpy;
  for (size_t i = 0; i < spinbox_euler_rpy_.size(); i++) {
    rpy.push_back(spinbox_euler_rpy_[i]->value());
  }
  config->setParameter(namespace_ + ".euler_rpy", rpy);
  return true;
}

void PoseSetting::transform(PointCloudT::Ptr cloud) {
  if (!enable_) {
    return;
  }
  pcl::transformPointCloud(*cloud, *cloud, pose_);
}

void PoseSetting::updatePoseSetting() {
  // enable/disable ui
  for (size_t i = 0; i < spinbox_euler_rpy_.size(); i++) {
    spinbox_euler_rpy_[i]->setEnabled(use_euler_angle_);
  }
  // update pose
  if (enable_) {
    for (int i = 0; i < 3; i++) {
      pose_(i, 3) = spinbox_translations_[i]->value();
    }
    Eigen::Matrix3f R = Eigen::Matrix3f::Identity();
    if (use_euler_angle_) {
      Eigen::Vector3f rpy;
      for (int i = 0; i < 3; i++) {
        rpy[i] = spinbox_euler_rpy_[i]->value() / 180 * 3.14159;
      }
      R = Eigen::AngleAxisf(rpy[2], Eigen::Vector3f::UnitZ()) *
          Eigen::AngleAxisf(rpy[1], Eigen::Vector3f::UnitY()) *
          Eigen::AngleAxisf(rpy[0], Eigen::Vector3f::UnitX());
    } else {
      for (int i = 0; i < 9; i++) {
        int row_idx = i / 3;
        int col_idx = i % 3;
        R(row_idx, col_idx) = spinbox_rotations_[i]->value();
      }
    }
    pose_.block<3, 3>(0, 0) = R;
    device_context_->refreshPointCloud();
  }
}

}  // namespace pointview
}  // namespace autox
