/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/14/24.
//

#ifndef POINTVIEW_BLIDAR_ADC_PLOT_H
#define POINTVIEW_BLIDAR_ADC_PLOT_H

#include <QMainWindow>
#include <QPushButton>

#include "utils/adc_plot.h"

namespace autox {
namespace pointview {

class BLidarAdcPlot : public AdcPlot {
  Q_OBJECT
  struct AdcParameter {
    int row;
    int column;
  };

 public:
  BLidarAdcPlot(std::shared_ptr<DeviceContext> device_context);
  ~BLidarAdcPlot();

 public slots:
  void set_position(int x, int y);

 protected slots:
  void SpinboxValueChangeSlot();

 protected:
  void ParseAdcPacket(const uint8_t* data, size_t len);
  std::string FillParameter(int f_p, int s_p);
  bool CheckPacket(const uint8_t* data, size_t len);
  QString GetSavePath(int index);
  double AdcToVolt(int16_t adc_value);

 private:
  // multi points setting
  AdcParameter current_parameter_;
};
}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_BLIDAR_ADC_PLOT_H
