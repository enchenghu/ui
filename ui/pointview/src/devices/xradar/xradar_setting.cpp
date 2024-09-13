
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xradar_setting.h"

#include "driver/arbe_driver.h"

using namespace std::chrono_literals;

namespace autox {
namespace pointview {

XRadarSetting::XRadarSetting(std::shared_ptr<DeviceContext> device_context)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  // create property
  auto sub = device_context->getPropertyTree()->createPropertySubTree(
      "device setting");

  // edit_deivce_ip_/edit_deivce_port_
  edit_deivce_ip_ = std::make_shared<QLineEdit>();
  edit_deivce_ip_->setText("127.0.0.1");
  sub->addProperty("device ip", edit_deivce_ip_);
  edit_deivce_port_ = std::make_shared<QLineEdit>();
  edit_deivce_port_->setText("6003");
  sub->addProperty("device port", edit_deivce_port_);
  // connect status and btn
  label_connect_status_ = std::make_shared<QLabel>();
  label_connect_status_->setText("disconnect!");
  sub->addProperty("status", label_connect_status_);
  btn_connect_ = std::make_shared<QPushButton>();
  btn_connect_->setText("connect");
  connect(btn_connect_.get(), &QPushButton::pressed, [this]() {
    if (!connected_) {
      if (connectTcp()) {
        connected_ = !connected_;
      }
    } else {
      driver_.reset();
      driver_ = std::make_shared<autox::drivers::arbe::ArbeDriver>();
      connected_ = !connected_;
    }
    updateWidegets();
  });
  sub->addProperty("operation", btn_connect_);
  // start/stop command
  btn_start_ = std::make_shared<QPushButton>();
  btn_start_->setText("start");
  connect(btn_start_.get(), &QPushButton::pressed,
          [this]() { driver_->startRadar(); });
  sub->addProperty("command", btn_start_);
  btn_stop_ = std::make_shared<QPushButton>();
  btn_stop_->setText("stop");
  connect(btn_stop_.get(), &QPushButton::pressed,
          [this]() { driver_->stopRadar(); });
  sub->addProperty("command", btn_stop_);
  // set threshold command
  auto threshold_sub = sub->createPropertySubTree("threshold");
  edit_power_threshold_ = std::make_shared<QSpinBox>();
  edit_power_threshold_->setValue(15);
  edit_power_threshold_->setRange(-10, 50);
  threshold_sub->addProperty("power", edit_power_threshold_);
  edit_azimuth_threshold_ = std::make_shared<QSpinBox>();
  edit_azimuth_threshold_->setValue(15);
  edit_power_threshold_->setRange(0, 50);
  threshold_sub->addProperty("azimuth", edit_azimuth_threshold_);
  edit_elevation_threshold_ = std::make_shared<QSpinBox>();
  edit_elevation_threshold_->setValue(3);
  edit_power_threshold_->setRange(0, 50);
  threshold_sub->addProperty("elevation", edit_elevation_threshold_);
  btn_set_params_ = std::make_shared<QPushButton>();
  btn_set_params_->setText("set threshold");
  threshold_sub->addProperty("command", btn_set_params_);
  connect(btn_set_params_.get(), &QPushButton::pressed, [this]() {
    int power = edit_power_threshold_->value();
    int azimuth = edit_azimuth_threshold_->value();
    int elevation = edit_elevation_threshold_->value();
    driver_->setThreshold(power, azimuth, elevation);
  });
  //
  driver_ = std::make_shared<autox::drivers::arbe::ArbeDriver>();
  updateWidegets();
}

XRadarSetting::~XRadarSetting() {}

void XRadarSetting::updateWidegets() {
  label_connect_status_->setText(connected_ ? "connected!" : "disconnected!");
  btn_connect_->setText(connected_ ? "disconnect" : "connect");
  edit_deivce_ip_->setEnabled(!connected_);
  edit_deivce_port_->setEnabled(!connected_);
  btn_start_->setEnabled(connected_);
  btn_stop_->setEnabled(connected_);
  btn_set_params_->setEnabled(connected_);
}

bool XRadarSetting::connectTcp() {
  std::string ip = edit_deivce_ip_->text().toStdString();
  int port = edit_deivce_port_->text().toInt();
  return driver_->connectRadarCtrl(ip, port);
}

bool XRadarSetting::initFromConfig(std::shared_ptr<Config> config) {
  std::string ip = "127.0.0.1";
  int port = 6003;
  int power = 15;
  int azimuth = 15;
  int elevation = 3;
  config->getParameter(namespace_ + ".device_ip", ip);
  edit_deivce_ip_->setText(QString::fromStdString(ip));
  config->getParameter(namespace_ + ".device_port", port);
  edit_deivce_port_->setText(QString::number(port));
  config->getParameter(namespace_ + ".threshold.power", power);
  edit_power_threshold_->setValue(power);
  config->getParameter(namespace_ + ".threshold.azimuth", azimuth);
  edit_azimuth_threshold_->setValue(azimuth);
  config->getParameter(namespace_ + ".threshold.elevation", elevation);
  edit_elevation_threshold_->setValue(elevation);
  return true;
}

bool XRadarSetting::storeToConfig(std::shared_ptr<Config> config) {
  config->setParameter(namespace_ + ".device_ip",
                       edit_deivce_ip_->text().toStdString());
  config->setParameter(namespace_ + ".device_port",
                       edit_deivce_port_->text().toInt());
  config->setParameter(namespace_ + ".threshold.power",
                       edit_power_threshold_->value());
  config->setParameter(namespace_ + ".threshold.azimuth",
                       edit_azimuth_threshold_->value());
  config->setParameter(namespace_ + ".threshold.elevation",
                       edit_elevation_threshold_->value());
  return true;
}

}  // namespace pointview
}  // namespace autox
