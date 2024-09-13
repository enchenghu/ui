/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_MESSAGE_DISPACHER
#define XRT_MESSAGE_DISPACHER

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace autox {
namespace xrt {
namespace record {
class RecordMessage;
}  // namespace record
}  // namespace xrt
}  // namespace autox

class XrtMessageDispatcher {
  using MessageQueue =
      std::deque<std::shared_ptr<autox::xrt::record::RecordMessage>>;

 private:
  XrtMessageDispatcher() = default;
  ~XrtMessageDispatcher() = default;

  // Singleton design pattern
 public:
  XrtMessageDispatcher(const XrtMessageDispatcher&) = delete;
  XrtMessageDispatcher(XrtMessageDispatcher&&) = delete;
  XrtMessageDispatcher& operator=(const XrtMessageDispatcher&) = delete;
  XrtMessageDispatcher& operator=(XrtMessageDispatcher&&) = delete;
  static XrtMessageDispatcher* getInstance() {
    static XrtMessageDispatcher instance;
    return &instance;
  }

 public:
  void reset();
  void addLidarChannelList(const std::vector<std::string>& channel_list);
  std::vector<std::string> getLidarChannelList();
  void addCameraChannelList(const std::vector<std::string>& channel_list);
  std::vector<std::string> getCameraChannelList();
  bool put(std::shared_ptr<autox::xrt::record::RecordMessage> msg);
  std::shared_ptr<autox::xrt::record::RecordMessage> get(
      const std::string& topic);

 private:
  void addChannelList(const std::vector<std::string>& channel_list);

 private:
  std::mutex mutex_;
  std::unordered_map<std::string, MessageQueue> message_buffer_;
  std::vector<std::string> lidar_channel_list_;
  std::vector<std::string> camera_channel_list_;
};

#endif  // XRT_MESSAGE_DISPACHER
