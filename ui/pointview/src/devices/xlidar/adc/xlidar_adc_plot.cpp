/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/27/24.
//

#include "xlidar_adc_plot.h"

#include "utils/utils.h"

struct PacketHeader {
  uint8_t sop[2];         // 0xEEFF 2bytes
  uint8_t major_version;  // 0xff
  uint8_t minor_version;  // 0x01
  uint16_t first_sample;
  uint16_t data_size;
  uint16_t azimuth_code;
  uint16_t adc_channel;
  uint16_t distance_group;
  uint16_t section;
  uint8_t status;
  uint8_t reserve[7];
};

struct AdcPacket {
  PacketHeader header;
  uint8_t data[0];
};

using namespace std::chrono_literals;

namespace autox {
namespace drivers {
namespace xlidar {

XLidarAdcPlot::XLidarAdcPlot(
    std::shared_ptr<autox::pointview::DeviceContext> device_context)
    : AdcPlot(device_context) {
  // parameter
  distance_group_.resize(256, 0);
  master_slavery_.resize(256, 0);
  adc_channel_.resize(256, 0);
  // azimuth code buffer
  azimuth_code_buffer_.resize(10000);
  azimuth_code_flag_.resize(10000);
  for (int i = 0; i < 10000; i++) {
    azimuth_code_buffer_[i].resize(256, 0);
    azimuth_code_flag_[i].resize(256, false);
  }
  setParameterTitle("Laser ID", "Azimuth Code");
  udp_receive_port_ = 2390;
  // init ui
  InitUi();
}

XLidarAdcPlot::~XLidarAdcPlot() {}

void XLidarAdcPlot::ParseAdcPacket(const uint8_t* data, size_t len) {
  // check packet
  if (!CheckPacket(data, len)) {
    return;
  }
  if (render_) {
    return;
  }
  // save adc data
  auto udp_pkt = reinterpret_cast<const AdcPacket*>(data);
  if (udp_pkt->header.status != 0) {
    set_status(false, udp_pkt->header.status);
    StopCollectData();
    return;
  }
  // check data valid
  if (udp_pkt->header.azimuth_code != current_parameter_.azimuth_code) return;
  if (udp_pkt->header.adc_channel != current_parameter_.adc_channel) return;
  if (udp_pkt->header.distance_group != current_parameter_.distance_group)
    return;

  adc_title_ =
      "laser id: " + std::to_string(current_parameter_.laser_id) +
      " , azimuth code: " + std::to_string(udp_pkt->header.azimuth_code) +
      " , channel: " + std::to_string(udp_pkt->header.adc_channel) +
      " , distance group: " + std::to_string(udp_pkt->header.distance_group);
  for (int i = 0; i < udp_pkt->header.data_size; i += 2) {
    uint16_t value =
        (static_cast<uint16_t>(udp_pkt->data[i + 1]) << 8) | udp_pkt->data[i];
    bool is_negative = (value & 0x8000) != 0;
    if (is_negative) {
      value = ~value + 1;
    }
    int16_t signed_value = static_cast<int16_t>(value);
    if (is_negative) {
      signed_value = -signed_value;
    }
    adc_data_.push_back(signed_value);
  }
  // recover ui
  if (adc_data_.size() >= need_sample_number_) {
    if (collect_mode_ == CollectMode::Multi) {
      collect_finish_ = true;
    } else {
      render_ = true;
    }
  }
}

bool XLidarAdcPlot::CheckPacket(const uint8_t* data,
                                size_t len) {  // check packet
  if (len <= 16) {
    return false;
  }
  // check head
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Invalid log packet, head need be 0xeeff: " << std::hex
       << (0xFF & header->sop[0]) << (0xFF & header->sop[1]) << std::dec;
    LOG(INFO) << ss.str();
    return false;
  }
  // check version
  if (header->major_version != 0x06 || header->minor_version != 0x01) {
    std::stringstream ss;
    ss << "Invalid log packet, version need be 6.1: " << std::hex
       << header->major_version << '.' << header->minor_version << std::dec;
    LOG(INFO) << ss.str();
    return false;
  }
  // check data size
  if (header->data_size + 24 != len) {
    std::stringstream ss;
    ss << "Invalid log packet, packet size need be data size + 24, "
          "packet size:"
       << len << ", data size:" << header->data_size;
    LOG(INFO) << ss.str();
    return false;
  }
  return true;
}

void XLidarAdcPlot::set_position(int x, int y) {
  int laser_id = y + 1;
  int azimuth_code = azimuth_code_buffer_[x][y];
  if (select_status_) {
    AddPoint(laser_id, azimuth_code);
  } else {
    if (getFirstParameter() != laser_id) {
      setFirstParameter(laser_id);
    }
    if (getSecondParameter() != azimuth_code) {
      setSecondParameter(azimuth_code);
    }
  }
}

void XLidarAdcPlot::OpenConfigSlot() {
  QString config_file = GetOpenFileName("Open Config File", last_open_dir_,
                                        "Csv File(*.csv);;All Files(*.*)");

  OpenIdReflectionConfig(config_file);
}

void XLidarAdcPlot::SetUiState(bool state) {
  autox::pointview::AdcPlot::SetUiState(state);
  open_config_button_->setEnabled(state);
}

int XLidarAdcPlot::adc_channel(int laser_id) { return adc_channel_[laser_id]; }

int XLidarAdcPlot::distance_group(int laser_id) {
  return distance_group_[laser_id];
}

void XLidarAdcPlot::set_azimuth_code(int x, int y, int azimuth_code) {
  if (azimuth_code_flag_.size() > x) {
    if (azimuth_code_flag_[x].size() > y) {
      if (!azimuth_code_flag_[x][y]) {
        azimuth_code_buffer_[x][y] = azimuth_code;
        azimuth_code_flag_[x][y] = true;
      }
    }
  }
}

void XLidarAdcPlot::OpenAzimuthCodeSlot() {
  QString azimuth_file = GetOpenFileName("Open Azimuth code", last_open_dir_,
                                         "Csv File(*.csv);;All Files(*.*)");
  if (azimuth_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  // open file
  ifstream f(azimuth_file.toStdString());
  std::string line;
  int j = 0;
  while (getline(f, line)) {
    std::stringstream ss(line);
    std::string str;
    for (int i = 0; i < 256; i++) {
      getline(ss, str, ',');
      azimuth_code_buffer_[j][i] = std::stoi(str);
      azimuth_code_flag_[j][i] = true;
    }
    j++;
  }
  azimuth_code_path_label_->getWidget()->setText(azimuth_file);
}

void XLidarAdcPlot::SaveAzimuthCodeSlot() {
  QString filename =
      GetSaveFileName("Save Azimuth code", QDir::homePath() + "/Untitled.csv",
                      "CSV Files(*.csv);;All Files(*.*)");
  if (filename.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  // open file
  std::ofstream out_file(filename.toStdString());
  if (!out_file) {
    return;
  }
  // save data
  for (int i = 0; i < azimuth_code_buffer_.size(); i++) {
    for (int j = 0; j < azimuth_code_buffer_[i].size(); j++) {
      out_file << azimuth_code_buffer_[i][j] << ",";
    }
    out_file;
  }
  QMessageBox::information(nullptr, "Adc Plot", "save CSV successfully!",
                           QMessageBox::Ok);
  LOG(INFO) << "save csv successfully :" + filename.toStdString();
}

void XLidarAdcPlot::SpinboxValueChangeSlot() {
  int y = getFirstParameter() - 1;
  int azimuth_code = getSecondParameter();
  int x = 0;
  for (; x < 10000; x++) {
    if (azimuth_code_buffer_[x][y] == azimuth_code) {
      break;
    }
  }
  if (x < 10000) {
    emit PostionValueChanged(x, y);
  }
}

bool XLidarAdcPlot::InitFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  autox::pointview::AdcPlot::InitFromConfig(config);
  std::string str;
  config->getParameter(namespace_ + ".id_reflection_config", str);
  OpenIdReflectionConfig(QString::fromStdString(str));
  return true;
}

bool XLidarAdcPlot::StoreToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  autox::pointview::AdcPlot::StoreToConfig(config);
  config->setParameter(namespace_ + ".id_reflection_config",
                       config_path_label_->getWidget()->text().toStdString());
  return true;
}

void XLidarAdcPlot::OpenIdReflectionConfig(QString config_path) {
  if (config_path.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  last_open_dir_ = QFileInfo(config_path).dir().absolutePath();
  adc_channel_.clear();
  distance_group_.clear();
  master_slavery_.clear();
  // open file
  ifstream f(config_path.toStdString());
  std::string line;
  while (getline(f, line)) {
    std::stringstream ss(line);
    std::string str;
    // laser id
    getline(ss, str, ',');
    // adc channel
    getline(ss, str, ',');
    adc_channel_.push_back(std::stoi(str));
    // distance group
    getline(ss, str, ',');
    distance_group_.push_back(std::stoi(str));
    // board(master, slaver)
    getline(ss, str, ',');
    master_slavery_.push_back(std::stoi(str));
  }
  config_path_label_->getWidget()->setText(config_path);
}

std::string XLidarAdcPlot::FillParameter(int f_p, int s_p) {
  std::string ip = getIp();
  current_parameter_.laser_id = f_p - 1;
  current_parameter_.azimuth_code = s_p;
  current_parameter_.distance_group =
      distance_group_[current_parameter_.laser_id];
  current_parameter_.master_slavery =
      master_slavery_[current_parameter_.laser_id];
  current_parameter_.adc_channel = adc_channel_[current_parameter_.laser_id];
  std::string url =
      ip + "/adc-info?Gt0x25&adcchannel=" +
      std::to_string(current_parameter_.adc_channel) +
      "&distancegroup=" + std::to_string(current_parameter_.distance_group) +
      "&section=" + std::to_string(current_parameter_.master_slavery) +
      "&sample=" + std::to_string(need_sample_number_) +
      "&udpport=" + std::to_string(udp_receive_port_) +
      "&azimuthcode=" + std::to_string(current_parameter_.azimuth_code);
  return url;
}

QString XLidarAdcPlot::GetSavePath(int index) {
  return save_dir_ + "/" + QString::number(index + 1) + "_" +
         QString::number(current_parameter_.laser_id) + "_" +
         QString::number(current_parameter_.azimuth_code) + ".csv";
}

double XLidarAdcPlot::AdcToVolt(int16_t adc_value) {
  return (adc_value / 65536.0 * std::pow(10, 0.75) * 1000.0);
}

void XLidarAdcPlot::InitUi() {
  // azimuth code
  // operation button
  open_azimuth_code_button_ = std::make_shared<QPushButton>("Open");
  save_azimuth_code_button_ = std::make_shared<QPushButton>("Save");
  h_layout_ = new QHBoxLayout();
  h_layout_->addWidget(open_azimuth_code_button_.get());
  h_layout_->addWidget(save_azimuth_code_button_.get());
  // label
  azimuth_code_path_label_ =
      std::make_shared<autox::pointview::XLabel>("Azimuth code Path:");
  // add layout
  azimuth_code_layout_ = std::make_shared<QVBoxLayout>();
  azimuth_code_layout_->addLayout(h_layout_);
  azimuth_code_layout_->addWidget(azimuth_code_path_label_.get());
  // group
  azimuth_code_group_ = new QGroupBox("Azimuth Code in File");
  azimuth_code_group_->setLayout(azimuth_code_layout_.get());
  addSettingWidget(azimuth_code_group_);
  connect(open_azimuth_code_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(OpenAzimuthCodeSlot()));
  connect(save_azimuth_code_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(SaveAzimuthCodeSlot()));
  // id config
  open_config_button_ = std::make_shared<QPushButton>("Open");
  config_path_label_ =
      std::make_shared<autox::pointview::XLabel>("Config Path:");
  config_group_ = new QGroupBox("Id Reflection Config");
  config_layout_ = std::make_shared<QVBoxLayout>();
  config_layout_->addWidget(open_config_button_.get());
  config_layout_->addWidget(config_path_label_.get());
  config_group_->setLayout(config_layout_.get());
  addSettingWidget(config_group_);
  connect(open_config_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(OpenConfigSlot()));
}

}  // namespace xlidar
}  // namespace drivers
}  // namespace autox