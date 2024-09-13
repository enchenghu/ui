/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef UDP_INPUT_H
#define UDP_INPUT_H

#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "pcap_udp_recorder.h"
#include "utils/common/device_context.h"

namespace autox {
namespace pointview {

struct UdpPacket {
  std::vector<uint8_t> data;
  int len;
};
class UdpInput {
  using PacketCallbackType = std::function<void(const uint8_t*, size_t)>;
  using UdpTimeoutCallbackType = std::function<void(int)>;

 public:
  UdpInput(std::shared_ptr<DeviceContext> device_context,
           bool use_dedicated_thread = false, size_t max_udp_len = 1500);
  ~UdpInput();
  void setUdpCallback(PacketCallbackType cb);
  void setMaxPacketQueueSize(size_t n);
  void setSocketRecvBufferSize(size_t n);
  void setTimeout(double t);
  bool open(uint16_t port);
  void close();
  bool isOpen();
  // recv data
  bool getPacket(uint8_t* data, size_t& len);
  bool poll();
  // for recorder
  void setMaxRecordPacket(size_t n);
  bool startRecorder();
  bool stopRecorder();
  void calcFpsAndTransSpeed(size_t n);
  float getUdpSpeed() const { return udpSpeed_; }

 private:
  bool is_open_{false};
  size_t max_udp_len_;
  // 1M = 1024*1024
  int socket_recv_buffer_size_{1048576};
  double timeout_{1.0};
  // for poll
  PacketCallbackType udp_cb_;
  UdpTimeoutCallbackType udp_timeout_cb_;
  std::vector<uint8_t> udp_data_;
  size_t udp_len_;
  int udp_port_;
  // pcap recorder
  std::shared_ptr<PcapUdpRecorder> pcap_recorder_;
  std::shared_ptr<DeviceContext> device_context_;
  // use dedicated thread for udp callback
  bool use_dedicated_thread_{false};
  size_t max_packet_depue_size_{2000};
  std::deque<std::shared_ptr<UdpPacket>> udp_packets_;
  std::deque<std::shared_ptr<UdpPacket>> unused_udp_packets_;
  std::unique_ptr<std::thread> dedicated_thread_;
  std::mutex udp_mutex_;
  std::mutex unused_udp_mutex_;
  bool exit_{false};
  float udpSpeed_{0.0};
  //  std::mutex socket_mutex_;
  int socket_fd_{-1};
};

}  // namespace pointview
}  // namespace autox

#endif  // UDP_INPUT_H
