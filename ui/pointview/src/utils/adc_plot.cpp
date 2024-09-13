/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 5/14/24.
//

#include "adc_plot.h"

#include <QMessageBox>
#include <QTime>

#include <curl/curl.h>

#include "ui_adc_plot.h"
#include "utils.h"

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

void Int2Bytes(int num, std::vector<uint8_t>& array) {
  uint16_t value = static_cast<uint16_t>(num);
  array.push_back(static_cast<uint8_t>(value >> 8));
  array.push_back(static_cast<uint8_t>(value & 0xff));
}

namespace autox {
namespace pointview {

AdcPlot::AdcPlot(std::shared_ptr<DeviceContext> device_context)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      ui_(new Ui::AdcPlotUi) {
  // ui
  ui_->setupUi(this);
  device_context_ = device_context;
  ui_->AdcWidget->addGraph();
  ui_->AdcWidget->xAxis->setLabel("Sample");
  ui_->AdcWidget->yAxis->setLabel("Volt(mv)");
  ui_->AdcWidget->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables |
                                  QCP::iRangeDrag);
  connect(ui_->AdcWidget, SIGNAL(mouseMove(QMouseEvent*)), this,
          SLOT(MouseHoveredSlot(QMouseEvent*)));
  cursor_y_ = new QCPItemLine(ui_->AdcWidget);
  cursor_x_ = new QCPItemLine(ui_->AdcWidget);
  ui_->MultiAddPointsPushButton->setCheckable(true);
  ui_->MultiAddPointsPushButton->setChecked(select_status_);
  // parameter
  distance_group_.resize(256, 0);
  master_slavery_.resize(256, 0);
  adc_channel_.resize(256, 0);
  // property
  open_button_ = std::make_shared<QPushButton>();
  open_button_->setText("Open");
  properties_.push_back({"Adc Sample", open_button_});
  connect(open_button_.get(), &QPushButton::clicked, [this]() { Open(); });
  device_context_->addProperties(properties_);
  // udp input
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    ParseAdcPacket(data, len);
  };
  udp_input_ = std::make_shared<autox::pointview::UdpInput>(device_context_);
  udp_input_->setUdpCallback(udp_cb);
  connect(ui_->CancelPushButton, SIGNAL(clicked(bool)), this,
          SLOT(StopCollectData()));
  // azimuth code buffer
  azimuth_code_buffer_.resize(10000);
  azimuth_code_flag_.resize(10000);
  for (int i = 0; i < 10000; i++) {
    azimuth_code_buffer_[i].resize(256, 0);
    azimuth_code_flag_[i].resize(256, false);
  }
  // init adc table
  for (int i = 0; i < 10; i++) {
    adc_data_.push_back(i);
  }
  ShowAdcData(adc_data_);
  connect(ui_->LaserIdSpinBox, SIGNAL(editingFinished()), this,
          SLOT(SpinboxValueChangeSlot()));
  connect(ui_->AzimuthCodeSpinBox, SIGNAL(editingFinished()), this,
          SLOT(SpinboxValueChangeSlot()));
  // player thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      // don't support playback
      if (udp_input_->isOpen()) {
        udp_input_->poll();
      } else {
        std::this_thread::sleep_for(1000ms);
      }
    }
  });
}

AdcPlot::~AdcPlot() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  curl_global_cleanup();
  delete ui_;
}

void AdcPlot::ParseAdcPacket(const uint8_t* data, size_t len) {
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

bool AdcPlot::CheckPacket(const uint8_t* data, size_t len) {  // check packet
  if (len <= 16) {
    return false;
  }
  // check head
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Invalid log packet, head need be 0xeeff: " << std::hex
       << (0xFF & header->sop[0]) << (0xFF & header->sop[1]) << std::dec;
    Debug(ss.str());
    return false;
  }
  // check version
  if (header->major_version != 0x06 || header->minor_version != 0x01) {
    std::stringstream ss;
    ss << "Invalid log packet, version need be 6.1: " << std::hex
       << header->major_version << '.' << header->minor_version << std::dec;
    Debug(ss.str());
    return false;
  }
  // check data size
  if (header->data_size + 24 != len) {
    std::stringstream ss;
    ss << "Invalid log packet, packet size need be data size + 24, "
          "packet size:"
       << len << ", data size:" << header->data_size;
    Debug(ss.str());
    return false;
  }
  return true;
}

void AdcPlot::set_position(int x, int y) {
  int laser_id = y + 1;
  int azimuth_code = azimuth_code_buffer_[x][y];
  if (select_status_) {
    AddPoint(laser_id, azimuth_code);
  } else {
    if (ui_->LaserIdSpinBox->value() != laser_id) {
      ui_->LaserIdSpinBox->setValue(laser_id);
    }
    if (ui_->AzimuthCodeSpinBox->value() != azimuth_code) {
      ui_->AzimuthCodeSpinBox->setValue(azimuth_code);
    }
  }
}

void AdcPlot::on_StartButton_clicked() {
  current_parameter_.laser_id = ui_->LaserIdSpinBox->value() - 1;
  current_parameter_.azimuth_code = ui_->AzimuthCodeSpinBox->value();
  current_parameter_.distance_group =
      distance_group_[current_parameter_.laser_id];
  current_parameter_.master_slavery =
      master_slavery_[current_parameter_.laser_id];
  current_parameter_.adc_channel = adc_channel_[current_parameter_.laser_id];
  need_sample_number_ = ui_->SampleNumberSpinBox->value();

  collect_mode_ = CollectMode::Single;
  if (!SendCollectRequest()) {
    StopCollectData();
  }
}

void AdcPlot::on_OpenFileButton_clicked() {
  QString config_file = GetOpenFileName("Open Config File", last_open_dir_,
                                        "Csv File(*.csv);;All Files(*.*)");

  OpenIdReflectionConfig(config_file);
}

void AdcPlot::Open() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void AdcPlot::SetUiState(bool state) {
  ui_->OpenAdcDataButton->setEnabled(state);
  ui_->SaveAdcDataButton->setEnabled(state);
  ui_->OpenFileButton->setEnabled(state);
  ui_->StartButton->setEnabled(state);
  ui_->CancelPushButton->setEnabled(!state);
}

void AdcPlot::ShowAdcData(std::vector<int16_t>& adc_data) {
  double min_value = adc_data[0] / 65536.0 * std::pow(10, 0.75) * 1000;
  double max_value = min_value;
  QVector<double> y;
  QVector<double> x;
  for (int i = 0; i < adc_data.size(); i++) {
    double value = adc_data[i] / 65536.0 * std::pow(10, 0.75) * 1000;
    min_value = std::min(value, min_value);
    max_value = std::max(value, max_value);
    x.append(i);
    y.append(value);
  }
  ui_->AdcWidget->graph(0)->setData(x, y);
  ui_->AdcWidget->setWindowTitle(QString::fromStdString(adc_title_));
  ui_->AdcWidget->xAxis->setRange(0, adc_data.size());
  ui_->AdcWidget->yAxis->setRange(min_value, max_value);
  ui_->AdcWidget->replot();
  set_status(true, 0);
}

void AdcPlot::Update() {
  if (!render_) {
    return;
  }
  if (adc_data_.size() >= need_sample_number_) {
    ShowAdcData(adc_data_);
    StopCollectData();
    render_ = false;
  }
}

void AdcPlot::MouseHoveredSlot(QMouseEvent* event) {
  if (adc_data_.empty()) {
    return;
  }
  double x = ui_->AdcWidget->xAxis->pixelToCoord(event->pos().x());
  // x cursor
  double start = ui_->AdcWidget->yAxis->pixelToCoord(0);
  double end =
      ui_->AdcWidget->yAxis->pixelToCoord(ui_->AdcWidget->size().height());
  cursor_y_->start->setCoords(x, start);
  cursor_y_->end->setCoords(x, end);
  // x cursor
  if (x < adc_data_.size() && x >= 0) {
    double y_value = adc_data_[int(x)] / 65536.0 * std::pow(10, 0.75) * 1000;
    double start_x = ui_->AdcWidget->xAxis->pixelToCoord(0);
    double end_x =
        ui_->AdcWidget->xAxis->pixelToCoord(ui_->AdcWidget->size().width());
    cursor_x_->start->setCoords(start_x, double(y_value));
    cursor_x_->end->setCoords(end_x, double(y_value));
    ui_->XLineEdit->setText(QString::asprintf("%2f", x));
    ui_->YLineEdit->setText(QString::asprintf("%2f", y_value));
  }
  ui_->AdcWidget->replot();
}

void AdcPlot::StopCollectData() {
  udp_input_->close();
  SetUiState(true);
}

int AdcPlot::adc_channel(int laser_id) { return adc_channel_[laser_id]; }

int AdcPlot::distance_group(int laser_id) { return distance_group_[laser_id]; }

void AdcPlot::on_OpenAdcDataButton_clicked() {
  QString adc_file = GetOpenFileName("Open Adc Data", last_open_dir_,
                                     "Csv File(*.csv);;All Files(*.*)");
  LoadAdcData(adc_file);
}

void AdcPlot::on_SaveAdcDataButton_clicked() {
  QString filename =
      GetSaveFileName("Save adc data", QDir::homePath() + "/Untitled.csv",
                      "CSV Files(*.csv);;All Files(*.*)");
  if (filename.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  SaveAdcData(filename);
}

void AdcPlot::set_azimuth_code(int x, int y, int azimuth_code) {
  if (!azimuth_code_flag_[x][y]) {
    azimuth_code_buffer_[x][y] = azimuth_code;
    azimuth_code_flag_[x][y] = true;
  }
}

void AdcPlot::on_OpenAzimuthCodeButton_clicked() {
  QString azimuth_file = GetOpenFileName("Open Azimuth code", last_open_dir_,
                                         "Csv File(*.csv);;All Files(*.*)");
  if (azimuth_file.isNull()) {
    Debug("Do not select a target file.");
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
}

void AdcPlot::on_SaveAzimuthCodeButton_clicked() {
  QString filename =
      GetSaveFileName("Save Azimuth code", QDir::homePath() + "/Untitled.csv",
                      "CSV Files(*.csv);;All Files(*.*)");
  if (filename.isNull()) {
    Debug("Do not select a target file.");
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
    out_file << std::endl;
  }
  QMessageBox::information(nullptr, "Adc Plot", "save CSV successfully!",
                           QMessageBox::Ok);
  Debug("save csv successfully :" + filename.toStdString());
}

void AdcPlot::set_status(bool status, int fail_code) {
  status_str_ = QTime::currentTime().toString();
  if (status) {
    status_str_ = status_str_ + " Success.";
  } else {
    status_str_ = status_str_ + " Fail! Code: " + QString::number(fail_code);
  }
  ui_->StatusLabel->setText(status_str_);
}

void AdcPlot::SpinboxValueChangeSlot() {
  int y = ui_->LaserIdSpinBox->value() - 1;
  int azimuth_code = ui_->AzimuthCodeSpinBox->value();
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

bool AdcPlot::InitFromConfig(std::shared_ptr<autox::pointview::Config> config) {
  int value;
  config->getParameter(namespace_ + ".laser_id", value);
  ui_->LaserIdSpinBox->setValue(value);
  config->getParameter(namespace_ + ".azimuth_code", value);
  ui_->AzimuthCodeSpinBox->setValue(value);
  config->getParameter(namespace_ + ".sample_number", value);
  ui_->SampleNumberSpinBox->setValue(value);
  std::string str;
  config->getParameter(namespace_ + ".ip", str);
  ui_->MasterIpLineEdit->setText(QString::fromStdString(str));
  config->getParameter(namespace_ + ".id_reflection_config", str);
  OpenIdReflectionConfig(QString::fromStdString(str));
  return true;
}

bool AdcPlot::StoreToConfig(std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter(namespace_ + ".laser_id", ui_->LaserIdSpinBox->value());
  config->setParameter(namespace_ + ".azimuth_code",
                       ui_->AzimuthCodeSpinBox->value());
  config->setParameter(namespace_ + ".sample_number",
                       ui_->SampleNumberSpinBox->value());
  config->setParameter(namespace_ + ".ip",
                       ui_->MasterIpLineEdit->text().toStdString());
  config->setParameter(namespace_ + ".id_reflection_config",
                       ui_->FileNameLabel->text().toStdString());
  return true;
}

void AdcPlot::OpenIdReflectionConfig(QString config_path) {
  if (config_path.isNull()) {
    Debug("Do not select a target file.");
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
  ui_->FileNameLabel->setText(config_path);
}

void AdcPlot::on_MultiStartPushButton_clicked() {
  // set mode
  collect_mode_ = CollectMode::Multi;
  // get new save path
  if (!GetMultiSavePushButton()) {
    return;
  }
  // progress bar
  QProgressDialog progress("Collect Adc Data...", "Abort Parse", 0,
                           multi_paramters_.size(), nullptr);
  progress.setWindowModality(Qt::WindowModal);
  progress.setValue(0);
  progress.show();

  for (int i = 0; i < multi_paramters_.size(); i++) {
    current_parameter_.laser_id = multi_paramters_[i].laser_id;
    current_parameter_.azimuth_code = multi_paramters_[i].azimuth_code;
    current_parameter_.distance_group =
        distance_group_[current_parameter_.laser_id];
    current_parameter_.master_slavery =
        master_slavery_[current_parameter_.laser_id];
    current_parameter_.adc_channel = adc_channel_[current_parameter_.laser_id];
    need_sample_number_ = ui_->SampleNumberSpinBox->value();

    collect_finish_ = false;
    progress.setValue(i);
    if (SendCollectRequest()) {
      // wait for data
      while (1) {
        if (progress.wasCanceled()) {
          break;
        }
        if (collect_finish_) {
          QString save_path =
              save_dir_ + "/" + QString::number(i + 1) + "_" +
              QString::number(current_parameter_.laser_id) + "_" +
              QString::number(current_parameter_.azimuth_code) + ".csv";
          SaveAdcData(save_path);
          set_status(true, 0);
          // write status
          QTableWidgetItem* item = new QTableWidgetItem();
          item->setData(Qt::DisplayRole, status_str_);
          ui_->ParameterTableWidget->setItem(i, 2, item);
          // write path
          QTableWidgetItem* item2 = new QTableWidgetItem();
          item2->setData(Qt::DisplayRole, save_path);
          ui_->ParameterTableWidget->setItem(i, 3, item2);
          break;
        }
        QApplication::processEvents();
      }
    } else {
      StopCollectData();
      break;
    }
    // cancel
    if (progress.wasCanceled()) {
      StopCollectData();
      break;
    }
    // recover status
    StopCollectData();
  }
  progress.setValue(multi_paramters_.size());
  emit SaveImage(save_dir_ + "/" + "range_image.png");
}

bool AdcPlot::SendCollectRequest() {
  if (ui_->FileNameLabel->text().isNull()) {
    QMessageBox::information(nullptr, "Adc Plot", "Please open config first!.",
                             QMessageBox::Ok);
    return false;
  }
  std::string ip = ui_->MasterIpLineEdit->text().toStdString();
  adc_data_.clear();
  // sent http request
  CURL* curl = curl_easy_init();
  if (curl) {
    // start udp receive
    udp_input_->open(udp_receive_port_);
    // freeze ui
    SetUiState(false);
    // send http request
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    std::string url =
        ip + "/adc-info?Gt0x25&adcchannel=" +
        std::to_string(current_parameter_.adc_channel) +
        "&distancegroup=" + std::to_string(current_parameter_.distance_group) +
        "&section=" + std::to_string(current_parameter_.master_slavery) +
        "&sample=" + std::to_string(need_sample_number_) +
        "&udpport=" + std::to_string(udp_receive_port_) +
        "&azimuthcode=" + std::to_string(current_parameter_.azimuth_code);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
    struct curl_slist* headers = NULL;
    headers =
        curl_slist_append(headers, "Content-Type:application/octet-stream");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res == CURLE_OK) {
      //      StopCollectData();
      return true;
    } else {
      QMessageBox::information(
          nullptr, "Adc Plot",
          "Curl failed! Curl response: " +
              QString::fromStdString(curl_easy_strerror(res)),
          QMessageBox::Ok);
    }
  }
  return false;
}

void AdcPlot::SaveAdcData(QString filename) {
  // open file
  std::ofstream out_file(filename.toStdString());
  if (!out_file) {
    return;
  }
  // save title
  out_file << adc_title_ << "," << std::endl;
  // save data
  for (int i = 0; i < adc_data_.size(); i++) {
    out_file << i << "," << adc_data_[i] << std::endl;
  }
  out_file.close();
  if (collect_mode_ == CollectMode::Single) {
    QMessageBox::information(nullptr, "Adc Plot", "save CSV successfully!",
                             QMessageBox::Ok);
    Debug("save csv successfully :" + filename.toStdString());
  }
}

bool AdcPlot::GetMultiSavePushButton() {
  save_dir_ =
      GetSaveDirectory("Choose folder for Recording File", QDir::currentPath());
  if (save_dir_.isNull()) {
    Debug("Do not select a target file.");
    return false;
  }
  ui_->MultiFilePathLabel->setText(save_dir_);
  return true;
}

void AdcPlot::on_MultiDeletePushButton_clicked() {
  int row_index = ui_->ParameterTableWidget->currentRow();
  if (row_index != -1 && multi_paramters_.size() > 0) {
    ui_->ParameterTableWidget->removeRow(row_index);
    multi_paramters_.erase(multi_paramters_.begin() + row_index);
  }
}

void AdcPlot::on_MultiAddPointsPushButton_clicked(bool status) {
  if (status != select_status_) {
    select_status_ = status;
    ui_->MultiAddPointsPushButton->setChecked(status);
    if (status == false) {
      ui_->ParameterTableWidget->setRowCount(0);
      multi_paramters_.clear();
    }
    emit SelectMultiPoints(status);
  }
}

void AdcPlot::on_AddPointPushButton_clicked() {
  int laser_id = ui_->LaserIdSpinBox->value() - 1;
  int azimuth_code = ui_->AzimuthCodeSpinBox->value();
  AddPoint(laser_id, azimuth_code);
}

void AdcPlot::LoadAdcData(QString adc_file) {
  if (adc_file.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  adc_data_.clear();
  // open file
  ifstream f(adc_file.toStdString());
  std::string line;
  // get title
  getline(f, adc_title_);
  while (getline(f, line)) {
    std::stringstream ss(line);
    std::string str;
    // laser id
    getline(ss, str, ',');
    // adc data
    getline(ss, str, ',');
    adc_data_.push_back(std::stoi(str));
  }
  ShowAdcData(adc_data_);
}

void AdcPlot::AddPoint(int laser_id, int azimuth_code) {
  int rowCount = ui_->ParameterTableWidget->rowCount();
  ui_->ParameterTableWidget->insertRow(rowCount);

  // laser_id
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(Qt::DisplayRole, laser_id);
  ui_->ParameterTableWidget->setItem(rowCount, 0, item);
  // azimuth_code
  QTableWidgetItem* item2 = new QTableWidgetItem();
  item2->setData(Qt::DisplayRole, azimuth_code);
  ui_->ParameterTableWidget->setItem(rowCount, 1, item2);
  // status
  QString init_str;
  QTableWidgetItem* item3 = new QTableWidgetItem();
  item3->setData(Qt::DisplayRole, init_str);
  ui_->ParameterTableWidget->setItem(rowCount, 2, item3);
  // path
  QTableWidgetItem* item4 = new QTableWidgetItem();
  item4->setData(Qt::DisplayRole, init_str);
  ui_->ParameterTableWidget->setItem(rowCount, 3, item4);
  // operation
  std::shared_ptr<QPushButton> button = std::make_shared<QPushButton>("open");
  connect(button.get(), SIGNAL(clicked(bool)), this,
          SLOT(OperationButtonClicked()));
  ui_->ParameterTableWidget->setCellWidget(rowCount, 4, button.get());
  multi_paramters_.push_back({laser_id, azimuth_code, button});
}

void AdcPlot::OperationButtonClicked() {
  int row_index = ui_->ParameterTableWidget->currentRow();
  QString file_path = ui_->ParameterTableWidget->item(row_index, 3)->text();
  Debug(file_path.toStdString());
  Debug(std::to_string(row_index));
  LoadAdcData(file_path);
}

}  // namespace pointview
}  // namespace autox
