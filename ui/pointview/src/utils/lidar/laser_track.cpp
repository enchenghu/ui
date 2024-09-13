//
// Created by tongdayang on 1/5/23.
//

#include "laser_track.h"

#include <iostream>

namespace autox {
namespace pointview {
LaserTrack::LaserTrack(std::shared_ptr<DeviceContext> device_context,
                       QStringList table_head)
    : PointTable(device_context->getDisplayContext()->getParent(), table_head,
                 "Laser Track"),
      open_button_(new QPushButton(this)),
      save_rule_button_(new QPushButton(this)) {
  device_context_ = device_context;
  open_button_->setText("Open");
  device_context_->addProperty("Laser Track", open_button_);
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  // create rule spin box
  start_channel_widget_ = std::make_shared<XSpinBox>("Start channel:");
  start_channel_widget_->getWidget()->setMaximum(256);
  end_channel_widget_ = std::make_shared<XSpinBox>("End channel:");
  end_channel_widget_->getWidget()->setMaximum(256);

  start_angle_widget_ = std::make_shared<XDoubleSpinBox>("Start angle:");
  start_angle_widget_->getWidget()->setMaximum(360);
  end_angle_widget_ = std::make_shared<XDoubleSpinBox>("End channel:");
  end_angle_widget_->getWidget()->setMaximum(360);
  custom_function_layout_->addWidget(start_channel_widget_.get());
  custom_function_layout_->addWidget(end_channel_widget_.get());
  custom_function_layout_->addWidget(start_angle_widget_.get());
  custom_function_layout_->addWidget(end_angle_widget_.get());
  // initFromConfig rule value
  rules_["start_channel"] = 1;
  rules_["end_channel"] = 1;
  rules_["start_angle"] = 0;
  rules_["end_angle"] = 3600;
  setRuletoSpinBox();
  // set save button
  save_rule_button_->setText("Refresh");
  connect(save_rule_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_saveRuleButtonClicked()));
  custom_function_layout_->addWidget(save_rule_button_.get());
}

LaserTrack::~LaserTrack() {}

void LaserTrack::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void LaserTrack::closeEvent(QCloseEvent* event) {
  PointTable::closeEvent(event);
  update_ = false;
}

void LaserTrack::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);
  update_ = true;
}

void LaserTrack::markPoint(PointT& p) {
  p.r = 255;
  p.g = 0;
  p.b = 0;
}

bool LaserTrack::checkPoint(int ch, float azimuth) {
  if (ch < rules_["start_channel"] || ch > rules_["end_channel"]) {
    return false;
  }
  int azimuth10 = azimuth * 10;
  if (azimuth10 < rules_["start_angle"] || azimuth10 > rules_["end_angle"]) {
    return false;
  }
  return true;
}

void LaserTrack::on_saveRuleButtonClicked() {
  int start_channel, end_channel, start_angle, end_angle;
  start_channel = start_channel_widget_->getWidget()->value();
  end_channel = end_channel_widget_->getWidget()->value();
  start_angle = start_angle_widget_->getWidget()->value() * 10;
  end_angle = end_angle_widget_->getWidget()->value() * 10;
  if (start_channel > end_channel || start_angle > end_angle) {
    std::cout << "error, reset to origin value" << std::endl;
    setRuletoSpinBox();
  } else {
    rules_["start_channel"] = start_channel;
    rules_["end_channel"] = end_channel;
    rules_["start_angle"] = start_angle;
    rules_["end_angle"] = end_angle;
  }
}

void LaserTrack::setRuletoSpinBox() {
  start_channel_widget_->getWidget()->setValue(rules_["start_channel"]);
  end_channel_widget_->getWidget()->setValue(rules_["end_channel"]);
  start_angle_widget_->getWidget()->setValue(rules_["start_angle"] / 10.0);
  end_angle_widget_->getWidget()->setValue(rules_["end_angle"] / 10.0);
}

bool LaserTrack::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  if (config) {
    config->getParameter(namespace_ + ".start_channel",
                         rules_["start_channel"]);
    config->getParameter(namespace_ + ".end_channel", rules_["end_channel"]);
    config->getParameter(namespace_ + ".start_angle", rules_["start_angle"]);
    config->getParameter(namespace_ + ".end_angle", rules_["end_angle"]);
    setRuletoSpinBox();
    return true;
  }
  return false;
}

bool LaserTrack::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  if (config) {
    config->setParameter(namespace_ + ".start_channel",
                         rules_["start_channel"]);
    config->setParameter(namespace_ + ".end_channel", rules_["end_channel"]);
    config->setParameter(namespace_ + ".start_angle", rules_["start_angle"]);
    config->setParameter(namespace_ + ".end_angle", rules_["end_angle"]);
    return true;
  }
  return false;
}

void LaserTrack::update(PointCloudT::Ptr cloud) {
  if (!update_) {
    return;
  }
  if (!point_info_cb_ || !point_check_cb_) {
    return;
  }
  cleanTable();
  std::vector<double> data;
  for (size_t i = 0; i < cloud->points.size(); i++) {
    if (point_check_cb_(i) && point_info_cb_(i, data)) {
      addPoint(data);
      markPoint(cloud->points[i]);
    }
  }
  refresh();
}
}  // namespace pointview
}  // namespace autox