/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef PCAP_UDP_RECORDER_H
#define PCAP_UDP_RECORDER_H

#include <algorithm>
#include <functional>
#include <limits>
#include <vector>
#include <thread>
#include "utils/common/safe_queue.h"
#include "utils/common/device_context.h"
#include <pcap.h>

namespace autox {
namespace pointview {
class PcapUdpRecorder {
 public:
  PcapUdpRecorder(std::shared_ptr<DeviceContext> device_context);
  ~PcapUdpRecorder() = default;

 public:
  void setRecordStatus(bool is_recording);
  void setMaxRecordPacket(size_t n);
  void startRecordTask();
  void stopRecordTask();
  void resetBufferQueue();
  size_t getByteCount();
  size_t getPacketCount();
  void addPacketData(const std::vector<uint8_t>& data);
  void addPacketData(const uint8_t* data, size_t len, int port);
  void clear();
  bool save(const std::string& file,
            std::function<bool(double)> progress_cb = nullptr);

 private:
  std::vector<uint8_t> pcap_udp_header_;
  std::vector<std::vector<uint8_t>> recorder_buffer_;
  SafeQueue<std::vector<uint8_t>> recorder_queue_free;
  SafeQueue<std::vector<uint8_t>> recorder_queue_done;
  size_t byte_cnt_{0};
  size_t max_record_udp_packet_number_{std::numeric_limits<size_t>::max()};
  bool is_recording_{false};
  std::unique_ptr<std::thread> save_thread_;
  pcap_dumper_t* fp_{nullptr};
  std::mutex save_mutex_;
  std::condition_variable save_ok_;
  std::shared_ptr<DeviceContext> device_context_;
  bool wait_record_{false};
  int record_port_{12345};
};
}  // namespace pointview
}  // namespace autox
#endif  // PCAP_UDP_RECORDER_H
