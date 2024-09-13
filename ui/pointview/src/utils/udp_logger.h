/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_UDP_LOGGER_H
#define POINTVIEW_UDP_LOGGER_H

#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <string>

#include "common/device_context.h"
#include "io/udp_input.h"
#include "log_viewer.h"

namespace autox::pointview {

class UdpLogger : public QObject {
  Q_OBJECT
 public:
  UdpLogger(std::shared_ptr<DeviceContext> device_context);
  ~UdpLogger();

 signals:
  void SendMessage(QString str);

 private:
  void pause();
  void parsePacket(const uint8_t* data, size_t len);
  bool checkPacket(const uint8_t* data, size_t len);

 private:
  std::shared_ptr<DeviceContext> device_context_;
  // namespace
  std::string namespace_{"udp log"};
  // items
  std::shared_ptr<QLineEdit> edit_udp_port_;
  std::shared_ptr<QPushButton> open_button_;
  // util and data
  std::shared_ptr<LogViewer> log_viewer_;
  std::shared_ptr<UdpInput> udp_input_;
  QString current_log_text_;
  // thread
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
};

}  // namespace autox::pointview

#endif  // POINTVIEW_UDP_LOGGER_H
