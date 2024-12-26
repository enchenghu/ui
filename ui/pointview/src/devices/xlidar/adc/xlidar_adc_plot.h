/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/27/24.
//

#ifndef POINTVIEW_XLIDAR_ADC_PLOT_H
#define POINTVIEW_XLIDAR_ADC_PLOT_H

#include <QLayout>
#include <QMainWindow>
#include <QPushButton>

#include "utils/adc_plot.h"
#include "utils/xqtwidget.h"

namespace autox {
namespace drivers {
namespace xlidar {

class XLidarAdcPlot : public autox::pointview::AdcPlot {
  Q_OBJECT
  struct AdcParameter {
    int laser_id;
    int azimuth_code;
    int distance_group;
    int master_slavery;
    int adc_channel;
  };

 public:
  XLidarAdcPlot(
      std::shared_ptr<autox::pointview::DeviceContext> device_context);
  ~XLidarAdcPlot();
  int adc_channel(int laser_id);
  int distance_group(int laser_id);
  void set_azimuth_code(int x, int y, int azimuth_code);
  bool InitFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool StoreToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 public slots:
  void set_position(int x, int y);

 protected slots:
  void SpinboxValueChangeSlot();
  void OpenConfigSlot();
  void OpenAzimuthCodeSlot();
  void SaveAzimuthCodeSlot();

 protected:
  void ParseAdcPacket(const uint8_t* data, size_t len);
  std::string FillParameter(int f_p, int s_p);
  bool CheckPacket(const uint8_t* data, size_t len);
  QString GetSavePath(int index);
  double AdcToVolt(int16_t adc_value);
  void OpenIdReflectionConfig(QString config_path);
  void SetUiState(bool state) override;
  void InitUi();

 private:
  // multi points setting
  AdcParameter current_parameter_;
  std::vector<int> adc_channel_;
  std::vector<int> distance_group_;
  std::vector<int> master_slavery_;
  // azimuth code store
  std::vector<std::vector<int>> azimuth_code_buffer_;
  std::vector<std::vector<bool>> azimuth_code_flag_;
  // azimuth code
  std::shared_ptr<QPushButton> open_azimuth_code_button_;
  std::shared_ptr<QPushButton> save_azimuth_code_button_;
  QHBoxLayout* h_layout_;
  std::shared_ptr<autox::pointview::XLabel> azimuth_code_path_label_;
  QGroupBox* azimuth_code_group_;
  std::shared_ptr<QVBoxLayout> azimuth_code_layout_;
  // id config
  std::shared_ptr<QPushButton> open_config_button_;
  std::shared_ptr<autox::pointview::XLabel> config_path_label_;
  QGroupBox* config_group_;
  std::shared_ptr<QVBoxLayout> config_layout_;
};

}  // namespace xlidar
}  // namespace drivers
}  // namespace autox

#endif  // POINTVIEW_XLIDAR_ADC_PLOT_H
