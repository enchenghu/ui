/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 5/14/24.
//

#ifndef POINTVIEW_ADC_PLOT_H
#define POINTVIEW_ADC_PLOT_H

#include <QMainWindow>
#include <QPushButton>

#include "common/device_context.h"
#include "config.h"
#include "io/udp_input.h"
#include "utils/qcustomplot/qcustomplot.h"

namespace Ui {
class AdcPlotUi;
}

namespace autox {
namespace pointview {

class AdcPlot : public QMainWindow {
  Q_OBJECT
  struct AdcParameter {
    int laser_id;
    int azimuth_code;
    int distance_group;
    int master_slavery;
    int adc_channel;
  };
  struct AdcTableItem {
    int laser_id;
    int azimuth_code;
    std::shared_ptr<QPushButton> button;
  };

  enum CollectMode { Single, Multi };

 public:
  AdcPlot(std::shared_ptr<DeviceContext> device_context);
  ~AdcPlot();
  void Update();
  int adc_channel(int laser_id);
  int distance_group(int laser_id);
  void set_azimuth_code(int x, int y, int azimuth_code);
  bool InitFromConfig(std::shared_ptr<autox::pointview::Config> config);
  bool StoreToConfig(std::shared_ptr<autox::pointview::Config> config);

 signals:
  void PostionValueChanged(int x, int y);
  void SelectMultiPoints(bool status);
  void SaveImage(QString file_name);

 public slots:
  void set_position(int x, int y);

 private slots:
  void on_StartButton_clicked();
  void on_OpenFileButton_clicked();
  void on_OpenAdcDataButton_clicked();
  void on_SaveAdcDataButton_clicked();
  void on_OpenAzimuthCodeButton_clicked();
  void on_SaveAzimuthCodeButton_clicked();
  void on_AddPointPushButton_clicked();
  void StopCollectData();
  void Open();
  void MouseHoveredSlot(QMouseEvent* event);
  void SpinboxValueChangeSlot();
  // multi points
  void on_MultiStartPushButton_clicked();
  void on_MultiDeletePushButton_clicked();
  void on_MultiAddPointsPushButton_clicked(bool status);
  void OperationButtonClicked();

 private:
  bool GetMultiSavePushButton();
  void ParseAdcPacket(const uint8_t* data, size_t len);
  bool CheckPacket(const uint8_t* data, size_t len);
  void SetUiState(bool state);
  void ShowAdcData(std::vector<int16_t>& adc_data);
  void set_status(bool status, int fail_code);
  void OpenIdReflectionConfig(QString config_path);
  bool SendCollectRequest();
  void SaveAdcData(QString filename);
  void LoadAdcData(QString adc_file);
  void AddPoint(int laser_id, int azimuth_code);

 private:
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
  std::vector<int> adc_channel_;
  std::vector<int> distance_group_;
  std::vector<int> master_slavery_;
  std::shared_ptr<UdpInput> udp_input_;
  int need_sample_number_;
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
  std::vector<int16_t> adc_data_;
  PropertyPairs properties_;
  // ui
  Ui::AdcPlotUi* ui_;
  QString last_open_dir_;
  bool render_{false};
  std::string adc_title_;
  int udp_receive_port_{2500};
  QCPItemLine* cursor_y_;
  QCPItemLine* cursor_x_;
  // azimuth code store
  std::vector<std::vector<int>> azimuth_code_buffer_;
  std::vector<std::vector<bool>> azimuth_code_flag_;
  std::string namespace_{"adc_plot"};
  // status string
  QString status_str_;
  // multi points setting
  AdcParameter current_parameter_;
  std::vector<AdcTableItem> multi_paramters_;
  CollectMode collect_mode_;
  QString save_dir_;
  bool collect_finish_{false};
  bool select_status_{false};
};
}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_ADC_PLOT_H
