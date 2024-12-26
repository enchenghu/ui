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
 public:
  struct AdcTableItem {
    int f_p;
    int s_p;
    std::shared_ptr<QPushButton> button;
  };

  enum CollectMode { Single, Multi };

 public:
  AdcPlot(std::shared_ptr<DeviceContext> device_context);
  ~AdcPlot();
  void Update();
  virtual bool InitFromConfig(std::shared_ptr<autox::pointview::Config> config);
  virtual bool StoreToConfig(std::shared_ptr<autox::pointview::Config> config);

 signals:
  void PostionValueChanged(int x, int y);
  void SelectMultiPoints(bool status);
  void SaveImage(QString file_name);

 public slots:
  virtual void set_position(int x, int y) = 0;

 protected slots:
  virtual void SpinboxValueChangeSlot() = 0;
  void on_StartButton_clicked();
  void on_OpenAdcDataButton_clicked();
  void on_SaveAdcDataButton_clicked();
  void on_AddPointPushButton_clicked();
  void StopCollectData();
  void Open();
  void MouseHoveredSlot(QMouseEvent* event);
  // multi points
  void on_MultiStartPushButton_clicked();
  void on_MultiDeletePushButton_clicked();
  void on_MultiAddPointsPushButton_clicked(bool status);
  void OperationButtonClicked();
  void on_LoopingStartButton_clicked();

 protected:
  virtual void ParseAdcPacket(const uint8_t* data, size_t len) = 0;
  virtual std::string FillParameter(int f_p, int s_p) = 0;
  virtual QString GetSavePath(int index) = 0;
  virtual double AdcToVolt(int16_t adc_value) = 0;
  virtual void SetUiState(bool state);
  bool getMultiSavePath();
  void ShowAdcData(std::vector<int16_t>& adc_data);
  void set_status(bool status, int fail_code);
  bool SendCollectRequest(std::string url);
  void SaveAdcData(QString filename);
  bool LoadAdcData(QString adc_file);
  void AddPoint(int f_p, int s_p);
  void setParameterTitle(QString f_t, QString s_t);
  int getFirstParameter();
  void setFirstParameter(int value);
  int getSecondParameter();
  void setSecondParameter(int value);
  std::string getIp();
  void addSettingWidget(QWidget* w);

 protected:
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<QPushButton> open_button_;
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
  std::string namespace_{"adc_plot"};
  // status string
  QString status_str_;
  // multi points setting
  std::vector<AdcTableItem> multi_paramters_;
  CollectMode collect_mode_;
  QString save_dir_;
  bool collect_finish_{false};
  bool select_status_{false};
};
}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_ADC_PLOT_H
