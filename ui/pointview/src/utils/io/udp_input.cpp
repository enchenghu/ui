
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "udp_input.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

using namespace std::literals::chrono_literals;

namespace autox {
namespace pointview {

UdpInput::UdpInput(std::shared_ptr<DeviceContext> device_context,
                   bool use_dedicated_thread, size_t max_udp_len) {
  device_context_ = device_context;
  use_dedicated_thread_ = use_dedicated_thread;
  max_udp_len_ = max_udp_len;
  // udp data
  udp_data_.resize(max_udp_len_);
  std::fill(udp_data_.begin(), udp_data_.end(), 0);
  // recorder
  pcap_recorder_ = std::make_shared<PcapUdpRecorder>(device_context_);
  // dedicated thread for udp callback
  if (use_dedicated_thread_) {
    // buffer for dedicated_thread
    setMaxPacketQueueSize(max_packet_depue_size_);
    // thread
    dedicated_thread_ = std::make_unique<std::thread>([this]() {
      std::shared_ptr<UdpPacket> pkt = nullptr;
      while (!exit_) {
        pkt = nullptr;
        {
          std::lock_guard<std::mutex> lock(udp_mutex_);
          if (!udp_packets_.empty()) {
            pkt = udp_packets_.front();
            udp_packets_.pop_front();
          }
        }
        if (pkt == nullptr) {
          std::this_thread::sleep_for(1ms);
          continue;
        }
        // record data and process callback
        pcap_recorder_->addPacketData(pkt->data.data(), pkt->len, udp_port_);
        udp_cb_(pkt->data.data(), pkt->len);
        {
          std::lock_guard<std::mutex> lk(unused_udp_mutex_);
          unused_udp_packets_.push_back(pkt);
        }
      }
    });
    pthread_setname_np(dedicated_thread_->native_handle(), "pv/udp_input");
  }
};

UdpInput::~UdpInput() {
  if (isOpen()) {
    this->close();
  }
  if (use_dedicated_thread_) {
    exit_ = true;
    dedicated_thread_->join();
    dedicated_thread_.reset();
  }
};

void UdpInput::setUdpCallback(PacketCallbackType cb) { udp_cb_ = cb; }

void UdpInput::setMaxPacketQueueSize(size_t n) {
  std::lock_guard<std::mutex> lock(unused_udp_mutex_);
  unused_udp_packets_.clear();
  max_packet_depue_size_ = n;
  // buffer
  for (size_t i = 0; i < max_packet_depue_size_; i++) {
    auto pkt = std::make_shared<UdpPacket>();
    pkt->data.resize(max_udp_len_);
    pkt->len = 0;
    unused_udp_packets_.push_back(pkt);
  }
}

void UdpInput::setSocketRecvBufferSize(size_t n) {
  socket_recv_buffer_size_ = n;
}

void UdpInput::setTimeout(double t) { timeout_ = t; }

bool UdpInput::open(uint16_t port) {
  if (isOpen()) {
    this->close();
  }
  LOG(INFO) << "Open port: " << port;
  //  std::lock_guard<std::mutex> lk(socket_mutex_);
  socket_fd_ = socket(PF_INET, SOCK_DGRAM, 0);
  if (socket_fd_ == -1) {
    LOG(INFO) << "Create socket failed.";
    return false;
  }
  if (timeout_ > 0) {
    struct timeval tv;
    tv.tv_sec = static_cast<int>(timeout_);
    tv.tv_usec = static_cast<int>((timeout_ - tv.tv_sec) * 1e6);
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
      LOG(INFO) << "Set timeout failed.";
      return false;
    }
  }
  // set receive buffer
  int recv_buf_size = socket_recv_buffer_size_;
  socklen_t optlen = sizeof(recv_buf_size);
  if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, optlen) <
      0) {
    LOG(INFO) << "Set socket buffer size error";
    return false;
  }
  // get receive buffer
  if (getsockopt(socket_fd_, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, &optlen) <
      0) {
    LOG(INFO) << "Get socket buffer size error";

    return false;
  }
  // set reuseable
  const int enable = 1;
  if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <
      0) {
    LOG(INFO) << "Setsockopt(SO_REUSEADDR) failed.";
  }
  LOG(INFO) << "Set udp socket recv buff size: " +
                   std::to_string(socket_recv_buffer_size_) + "->" +
                   std::to_string(recv_buf_size / 2);

  sockaddr_in myAddress;                          // my address information
  memset(&myAddress, 0, sizeof(myAddress));       // initialize to zeros
  myAddress.sin_family = AF_INET;                 // host byte order
  myAddress.sin_port = htons(port);               // port in network byte order
  myAddress.sin_addr.s_addr = htonl(INADDR_ANY);  // automatically fill in my IP
  if (bind(socket_fd_, reinterpret_cast<sockaddr*>(&myAddress),
           sizeof(sockaddr)) == -1) {
    LOG(INFO) << "Socket bind failed";
    return false;
  }
  udp_port_ = port;
  is_open_ = true;
  return true;
}

void UdpInput::close() {
  if (is_open_) {
    //    std::lock_guard<std::mutex> lk(socket_mutex_);
    ::close(socket_fd_);
    socket_fd_ = -1;
    is_open_ = false;
  }
}

bool UdpInput::isOpen() { return is_open_; }

bool UdpInput::getPacket(uint8_t* data, size_t& len) {
  if (data == nullptr) {
    return false;
  }
  //  std::lock_guard<std::mutex> lk(socket_mutex_);
  sockaddr_in senderAddress;
  socklen_t senderAddressLen = sizeof(senderAddress);
  ssize_t nbytes =
      recvfrom(socket_fd_, data, max_udp_len_, 0,
               reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressLen);
  if (nbytes <= 0) {
    udpSpeed_ = 0.0;
    return false;
  }
  calcFpsAndTransSpeed(nbytes);
  len = static_cast<size_t>(nbytes);
  return true;
}

bool UdpInput::poll() {
  if (!udp_cb_) {
    std::cerr << "[UdpInput] no udp callback.";
    std::this_thread::sleep_for(10ms);
    return false;
  }
  if (!use_dedicated_thread_) {
    // poll without dedicated thread
    if (!getPacket(udp_data_.data(), udp_len_)) {
      return false;
    }
    // record data and process callback
    pcap_recorder_->addPacketData(udp_data_.data(), udp_len_, udp_port_);
    udp_cb_(udp_data_.data(), udp_len_);
  } else {
    // poll with dedicated thread for udp callback
    std::shared_ptr<UdpPacket> pkt;
    {
      std::lock_guard<std::mutex> lock(unused_udp_mutex_);
      if (unused_udp_packets_.empty()) {
        std::lock_guard<std::mutex> lock(udp_mutex_);
        while (!udp_packets_.empty()) {
          unused_udp_packets_.push_back(udp_packets_.front());
          udp_packets_.pop_front();
        }
        LOG(INFO) << "[UdpInput] warning, udp deque is full, clear all buffer.";
        return false;
      }
      pkt = unused_udp_packets_.front();
    }
    if (!getPacket(pkt->data.data(), udp_len_)) {
      return false;
    }
    pkt->len = udp_len_;
    {
      std::lock_guard<std::mutex> lock(udp_mutex_);
      udp_packets_.push_back(pkt);
    }
    {
      std::lock_guard<std::mutex> lock(unused_udp_mutex_);
      unused_udp_packets_.pop_front();
    }
  }
  return true;
}

void UdpInput::setMaxRecordPacket(size_t n) {
  pcap_recorder_->setMaxRecordPacket(n);
}

bool UdpInput::startRecorder() {
  pcap_recorder_->clear();
  pcap_recorder_->setRecordStatus(true);
  pcap_recorder_->startRecordTask();
  return true;
}

bool UdpInput::stopRecorder() {
  pcap_recorder_->setRecordStatus(false);
  pcap_recorder_->stopRecordTask();
  // save record udp packets
  /*   LOG(INFO) << "[UdpInput] recorder_buffer size:"
              << pcap_recorder_->getPacketCount() ;
    if (pcap_recorder_->getPacketCount() == 0) {
      return true;
    }
    if (pcap_recorder_->save(save_file_path, progress_cb)) {
      LOG(INFO) << "[UdpInput] save recorder successfully!" ;
    } */
  return true;
}

void UdpInput::calcFpsAndTransSpeed(size_t n) {
  if (n < 0) return;
  static auto last = std::chrono::steady_clock::now();
  static long byteNum = 0;
  std::chrono::duration<double> elapsed;
  byteNum += n;
  auto current = std::chrono::steady_clock::now();
  elapsed = current - last;
  if (elapsed.count() > 3) {
    last = current;
    float res = (float)(byteNum) / (elapsed.count());
    udpSpeed_ = res / 1024.0;
    byteNum = 0;
  }
}

}  // namespace pointview
}  // namespace autox
