/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt_message_dispatcher.h"

#include <iostream>

#include <glog/logging.h>

#include "xrt/record/record_message.h"

void XrtMessageDispatcher::reset() {
  std::lock_guard<std::mutex> lock(mutex_);
  message_buffer_.clear();
  lidar_channel_list_.clear();
}

void XrtMessageDispatcher::addChannelList(
    const std::vector<std::string>& channel_list) {
  for (auto& channel : channel_list) {
    if (message_buffer_.find(channel) != message_buffer_.end()) {
      LOG(INFO) << "add repeated channel name:" << channel;
      continue;
    }
    MessageQueue q;
    message_buffer_.emplace(channel, q);
  }
}

void XrtMessageDispatcher::addLidarChannelList(
    const std::vector<std::string>& channel_list) {
  std::lock_guard<std::mutex> lock(mutex_);
  addChannelList(channel_list);
  lidar_channel_list_ = channel_list;
}

std::vector<std::string> XrtMessageDispatcher::getLidarChannelList() {
  std::lock_guard<std::mutex> lock(mutex_);
  return lidar_channel_list_;
}

void XrtMessageDispatcher::addCameraChannelList(
    const std::vector<std::string>& channel_list) {
  std::lock_guard<std::mutex> lock(mutex_);
  addChannelList(channel_list);
  camera_channel_list_ = channel_list;
}

std::vector<std::string> XrtMessageDispatcher::getCameraChannelList() {
  std::lock_guard<std::mutex> lock(mutex_);
  return camera_channel_list_;
}

bool XrtMessageDispatcher::put(
    std::shared_ptr<autox::xrt::record::RecordMessage> msg) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto messages = message_buffer_.find(msg->channel_name);
  if (messages != message_buffer_.end()) {
    messages->second.push_back(msg);
    // queue full
    if (messages->second.size() > 3) {
      messages->second.pop_front();
    }
    return true;
  }
  return false;
}

std::shared_ptr<autox::xrt::record::RecordMessage> XrtMessageDispatcher::get(
    const std::string& topic) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto messages = message_buffer_.find(topic);
  if (messages != message_buffer_.end()) {
    if (!messages->second.empty()) {
      auto msg = messages->second.front();
      messages->second.pop_front();
      return msg;
    }
  }
  return nullptr;
}
