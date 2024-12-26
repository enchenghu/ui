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

using namespace std::chrono_literals;

namespace autox {
namespace pointview {

AdcPlot::AdcPlot(std::shared_ptr<DeviceContext> device_context)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      ui_(new Ui::AdcPlotUi) {
  // ui
  ui_->setupUi(this);
  device_context_ = device_context;
  ui_->TimeGapBox->setMaximum(10000);
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
  // init adc table
  for (int i = 0; i < 10; i++) {
    adc_data_.push_back(i);
  }
  //  ShowAdcData(adc_data_);
  connect(ui_->FirstPSpinBox, SIGNAL(editingFinished()), this,
          SLOT(SpinboxValueChangeSlot()));
  connect(ui_->SecondPSpinBox, SIGNAL(editingFinished()), this,
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

void AdcPlot::on_StartButton_clicked() {
  need_sample_number_ = ui_->SampleNumberSpinBox->value();
  std::string url =
      FillParameter(ui_->FirstPSpinBox->value(), ui_->SecondPSpinBox->value());
  collect_mode_ = CollectMode::Single;
  if (!SendCollectRequest(url)) {
    StopCollectData();
  }
}

void AdcPlot::Open() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void AdcPlot::SetUiState(bool state) {
  ui_->OpenAdcDataButton->setEnabled(state);
  ui_->SaveAdcDataButton->setEnabled(state);
  ui_->StartButton->setEnabled(state);
  ui_->CancelPushButton->setEnabled(!state);
}

void AdcPlot::ShowAdcData(std::vector<int16_t>& adc_data) {
  double min_value = AdcToVolt(adc_data[0]);
  double max_value = min_value;
  QVector<double> y;
  QVector<double> x;
  for (int i = 0; i < adc_data.size(); i++) {
    double value = AdcToVolt(adc_data[i]);
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
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  SaveAdcData(filename);
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

bool AdcPlot::InitFromConfig(std::shared_ptr<autox::pointview::Config> config) {
  int value;
  config->getParameter(namespace_ + ".f_p", value);
  ui_->FirstPSpinBox->setValue(value);
  config->getParameter(namespace_ + ".s_p", value);
  ui_->SecondPSpinBox->setValue(value);
  config->getParameter(namespace_ + ".sample_number", value);
  ui_->SampleNumberSpinBox->setValue(value);
  std::string str;
  config->getParameter(namespace_ + ".ip", str);
  ui_->IpLineEdit->setText(QString::fromStdString(str));
  return true;
}

bool AdcPlot::StoreToConfig(std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter(namespace_ + ".f_p", ui_->FirstPSpinBox->value());
  config->setParameter(namespace_ + ".s_p", ui_->SecondPSpinBox->value());
  config->setParameter(namespace_ + ".sample_number",
                       ui_->SampleNumberSpinBox->value());
  config->setParameter(namespace_ + ".ip",
                       ui_->IpLineEdit->text().toStdString());
  return true;
}

void AdcPlot::on_MultiStartPushButton_clicked() {
  // set mode
  collect_mode_ = CollectMode::Multi;
  // get new save path
  if (!getMultiSavePath()) {
    return;
  }
  // progress bar
  QProgressDialog progress("Collect Adc Data...", "Abort Parse", 0,
                           multi_paramters_.size(), nullptr);
  progress.setWindowModality(Qt::WindowModal);
  progress.setValue(0);
  progress.show();
  need_sample_number_ = ui_->SampleNumberSpinBox->value();
  for (int i = 0; i < multi_paramters_.size(); i++) {
    std::string url =
        FillParameter(multi_paramters_[i].f_p, multi_paramters_[i].s_p);
    collect_finish_ = false;
    progress.setValue(i);
    if (SendCollectRequest(url)) {
      // wait for data
      while (1) {
        if (progress.wasCanceled()) {
          break;
        }
        if (collect_finish_) {
          QString save_path = GetSavePath(i);
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

bool AdcPlot::SendCollectRequest(std::string url) {
  std::string ip = ui_->IpLineEdit->text().toStdString();
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
    LOG(INFO) << "send request :" + url;
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
    LOG(INFO) << "save csv successfully :" + filename.toStdString();
  }
}

bool AdcPlot::getMultiSavePath() {
  save_dir_ =
      GetSaveDirectory("Choose folder for Recording File", QDir::currentPath());
  if (save_dir_.isNull()) {
    LOG(INFO) << "Do not select a target file.";
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
  int f_p = ui_->FirstPSpinBox->value();
  int s_p = ui_->SecondPSpinBox->value();
  AddPoint(f_p, s_p);
}

bool AdcPlot::LoadAdcData(QString adc_file) {
  if (adc_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return false;
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
  ui_->AdcPathLabel->setText(adc_file);
  return true;
}

void AdcPlot::AddPoint(int f_p, int s_p) {
  int rowCount = ui_->ParameterTableWidget->rowCount();
  ui_->ParameterTableWidget->insertRow(rowCount);

  // laser_id
  QTableWidgetItem* item = new QTableWidgetItem();
  item->setData(Qt::DisplayRole, f_p);
  ui_->ParameterTableWidget->setItem(rowCount, 0, item);
  // azimuth_code
  QTableWidgetItem* item2 = new QTableWidgetItem();
  item2->setData(Qt::DisplayRole, s_p);
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
  multi_paramters_.push_back({f_p, s_p, button});
}

void AdcPlot::OperationButtonClicked() {
  int row_index = ui_->ParameterTableWidget->currentRow();
  QString file_path = ui_->ParameterTableWidget->item(row_index, 3)->text();
  LOG(INFO) << file_path.toStdString();
  LOG(INFO) << std::to_string(row_index);
  LoadAdcData(file_path);
}

void AdcPlot::setParameterTitle(QString f_t, QString s_t) {
  ui_->FirstParameterLabel->setText(f_t + ":");
  ui_->SecondParameterLabel->setText(s_t + ":");
  QStringList head = {f_t, s_t, "Status", "Save path", "Operation"};
  ui_->ParameterTableWidget->setColumnCount(5);
  ui_->ParameterTableWidget->setHorizontalHeaderLabels(head);
}

int AdcPlot::getFirstParameter() { return ui_->FirstPSpinBox->value(); }

void AdcPlot::setFirstParameter(int value) {
  ui_->FirstPSpinBox->setValue(value);
}

int AdcPlot::getSecondParameter() { return ui_->SecondPSpinBox->value(); }

void AdcPlot::setSecondParameter(int value) {
  ui_->SecondPSpinBox->setValue(value);
}
std::string AdcPlot::getIp() { return ui_->IpLineEdit->text().toStdString(); }

void AdcPlot::addSettingWidget(QWidget* w) { ui_->SettingLayout->addWidget(w); }

void AdcPlot::on_LoopingStartButton_clicked() {
  int times = ui_->LoopTimesBox->value();
  int time_gap = ui_->TimeGapBox->value();
  // progress bar
  QProgressDialog progress("Collect Adc Data...", "Abort Parse", 0, times,
                           nullptr);
  progress.setWindowModality(Qt::WindowModal);
  progress.setValue(0);
  progress.show();
  for (int i = 0; i < times; i++) {
    progress.setValue(i);
    on_StartButton_clicked();
    std::this_thread::sleep_for(std::chrono::milliseconds(time_gap));
    // cancel
    if (progress.wasCanceled()) {
      StopCollectData();
      break;
    }
  }
  progress.setValue(times);
}

}  // namespace pointview
}  // namespace autox
