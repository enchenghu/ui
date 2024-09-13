/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef SIMPLE_PLAYER_SETTING_H
#define SIMPLE_PLAYER_SETTING_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <condition_variable>
#include <memory>
#include <string>
#include <vector>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/io/udp_input.h"

namespace autox {
namespace pointview {

class SimplePlayerSetting : public QObject {
  Q_OBJECT

 public:
  SimplePlayerSetting(std::shared_ptr<DeviceContext> device_context,
                      std::shared_ptr<UdpInput> udp_input,
                      std::shared_ptr<PcapUdpParser> pcap_parser,
                      std::shared_ptr<PlaybackBufferBase> playback_buffer);
  ~SimplePlayerSetting();
  bool initFromConfig(std::shared_ptr<Config> config);
  bool storeToConfig(std::shared_ptr<Config> config);
  void setSyncPlayerCb(std::function<void()> cb);
  void setResetDriverCb(std::function<void(bool)> cb);
  void setDriverThreadCb(std::function<void(int)> cb) {
    set_driver_thread_cb_ = cb;
  }
  void setDriverThreadNum(int n) { driver_thread_num_ = n; }
  bool startPlayer();
  void refreshUdpSpeed();
  void updateUdpIndexPcap(size_t begin, size_t end);
  void setUdpPort(std::string port);

 private slots:
  void playerTypeChanged(int index);
  bool openPcapClicked();
  bool openPcapMemoryClicked();
  void setRecordPath();

 private:
  bool parsePcap();

 private:
  //
  std::shared_ptr<DeviceContext> device_context_;
  std::shared_ptr<UdpInput> udp_input_;
  std::shared_ptr<PcapUdpParser> pcap_parser_;
  std::shared_ptr<PlaybackBufferBase> playback_buffer_;
  // item
  std::shared_ptr<QComboBox> combobox_player_type_;
  std::shared_ptr<QLineEdit> edit_udp_port_;
  std::shared_ptr<QPushButton> btn_pcap_file_;
  std::shared_ptr<QPushButton> btn_mem_pcap_file_;
  std::shared_ptr<QLabel> label_pcap_file_;
  std::shared_ptr<QLabel> label_udp_speed_;
  std::shared_ptr<QLabel> label_packet_index_;
  std::shared_ptr<QCheckBox> checkbox_enable_pcap_filter_;
  std::shared_ptr<QLineEdit> edit_pcap_filter_udp_port_;
  std::shared_ptr<QCheckBox> checkbox_enable_constant_fps_;
  std::shared_ptr<QDoubleSpinBox> spinbox_constant_fps_;
  std::shared_ptr<QPushButton> btn_record_path_;
  std::shared_ptr<QLabel> label_record_path_;
  // property namespace
  std::string namespace_{"player_setting"};
  // thread
  std::unique_ptr<std::thread> player_thread_;
  std::unique_ptr<std::thread> parse_all_thread_;
  std::mutex read_packets_mutex_;
  std::condition_variable read_packets_ok_;
  bool exit_{false};
  bool exit_a{false};
  // sync player callback
  std::function<void()> sync_player_cb_;
  std::function<void(bool)> reset_driver_cb_;
  std::function<void(int)> set_driver_thread_cb_;
  bool use_constant_fps_{false};
  double constant_fps_{10};
  int driver_thread_num_{0};
  //
  QString last_pcap_open_dirpath_;
  QString last_pcap_memory_open_dirpath_;
};

}  // namespace pointview
}  // namespace autox

#endif  // SIMPLE_PLAYER_SETTING_H
