#ifndef UDP_REPLAYER_H
#define UDP_REPLAYER_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "utils/common/device_base.h"
#include "utils/io/pcap_udp_parser.h"
#include "utils/io/playback_buffer.h"
#include "utils/utils.h"

class UdpReplayer : public autox::pointview::DeviceBase {
  Q_OBJECT
 public:
  struct UdpPacket {
    std::vector<uint8_t> data;
    size_t len;
    int port;
  };
  using PlaybackBuffer = autox::pointview::PlaybackBuffer<UdpPacket>;
  UdpReplayer(std::shared_ptr<autox::pointview::DisplayContext> context,
              autox::pointview::DeviceBaseParameter& parameter);
  ~UdpReplayer() override;
  bool updateUI() override { return true; };
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 private slots:
  void openPcapClicked();

 private:
  bool parsePcap(const std::string& pcap_file);
  bool pausePlayer();

 private:
  // Ui object
  std::shared_ptr<QPushButton> btn_pcap_file_;
  std::shared_ptr<QLabel> label_pcap_file_;
  std::shared_ptr<QCheckBox> checkbox_enable_constant_fps_;
  std::shared_ptr<QDoubleSpinBox> spinbox_constant_fps_;
  std::shared_ptr<QComboBox> lidar_type_fps_;
  std::shared_ptr<QCheckBox> checkbox_custom_ip_;
  std::shared_ptr<QLineEdit> custom_ip_edit_;
  // for pcap playback
  std::shared_ptr<autox::pointview::PcapUdpParser> pcap_parser_;
  std::shared_ptr<PlaybackBuffer> playback_buffer_;
  int udp_sock_ = -1;
  // thread
  std::unique_ptr<std::thread> player_thread_;
  std::unique_ptr<std::thread> parse_all_thread_;
  bool exit_{false};
  bool exit_parse_{false};
  std::mutex read_packets_mutex_;
  std::condition_variable read_packets_ok_;
  //
  QString last_pcap_open_dirpath_;
  // constant fps
  bool use_constant_fps_{false};
  double constant_fps_{1000};
  // custom ip
  bool use_custom_ip_{false};
  size_t packet_cnt_{0};
};

#endif  // UDP_REPLAYER_H
