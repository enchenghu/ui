/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "udp_logger.h"

#include <glog/logging.h>

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding
typedef struct {
  uint8_t sop[2];         // 0xEEFF 2bytes
  uint8_t major_version;  // 0xff
  uint8_t minor_version;  // 0x01
  uint16_t data_size;
} PacketHeader;

typedef struct {
  PacketHeader header;
  // data
  char data[0];
} LogPacket;

#pragma pack(pop)

using namespace std::chrono_literals;

namespace autox::pointview {
UdpLogger::UdpLogger(std::shared_ptr<DeviceContext> device_context)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  log_viewer_ = std::make_unique<autox::pointview::LogViewer>(
      device_context->getDisplayContext()->getParent());
  connect(this, SIGNAL(SendMessage(QString)), log_viewer_.get(),
          SLOT(addText(const QString)));
  // property
  auto sub =
      device_context_->getPropertyTree()->createPropertySubTree("udp log");
  edit_udp_port_ = std::make_shared<QLineEdit>();
  edit_udp_port_->setText("89123");
  sub->addProperty("udp port", edit_udp_port_);
  open_button_ = std::make_shared<QPushButton>();
  open_button_->setText("Open");
  sub->addProperty("viewer", open_button_);
  connect(open_button_.get(), &QPushButton::clicked,
          [this]() { log_viewer_->open(); });
  // udp input
  auto udp_cb = [this](const uint8_t* data, size_t len) {
    parsePacket(data, len);
  };
  udp_input_ = std::make_shared<autox::pointview::UdpInput>(device_context_);
  udp_input_->setUdpCallback(udp_cb);
  // set player callback
  device_context_->registerStartPlayerCb([this]() {
    auto play_state = device_context_->getPlayerState();
    if (play_state.is_playback) {
      return false;
    }
    std::string port = edit_udp_port_->text().toStdString();
    if (port.empty()) {
      return false;
    }
    udp_input_->open(stoi(port));
    edit_udp_port_->setEnabled(false);
    return true;
  });
  device_context_->registerPausePlayerCb([this]() -> void {
    auto play_state = device_context_->getPlayerState();
    if (play_state.is_playback) {
      return;
    }
    edit_udp_port_->setEnabled(true);
    udp_input_->close();
  });

  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      // don't support playback
      if (udp_input_->isOpen() && !play_state.is_playback &&
          play_state.is_playing) {
        udp_input_->poll();
      } else {
        std::this_thread::sleep_for(1000ms);
      }
    }
  });
}

UdpLogger::~UdpLogger() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
}

void UdpLogger::parsePacket(const uint8_t* data, size_t len) {
  // check packet
  if (!checkPacket(data, len)) {
    return;
  }
  // copy to current_log_text_
  auto udp_pkt = reinterpret_cast<const LogPacket*>(data);
  current_log_text_ =
      QString::fromLocal8Bit(udp_pkt->data, udp_pkt->header.data_size);
  // update log viewer
  emit SendMessage(current_log_text_);
}

bool UdpLogger::checkPacket(const uint8_t* data, size_t len) {
  // check packet
  if (len <= 8) {
    return false;
  }
  // check head
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    LOG(INFO) << "Invalid log packet, head need be 0xeeff: " << std::hex
              << (0xFF & header->sop[0]) << (0xFF & header->sop[1]) << std::dec;
    return false;
  }
  // check version
  if (header->major_version != 0xff || header->minor_version != 0x01) {
    LOG(INFO) << "Invalid log packet, version need be 255.1: " << std::hex
              << header->major_version << '.' << header->minor_version
              << std::dec;
    return false;
  }
  // check data size
  if (header->data_size + 8 != len) {
    LOG(INFO)
        << "Invalid log packet, packet size need be data size + 8, packet size:"
        << len << ", data size:" << header->data_size;
    return false;
  }
  // check tail
  if (data[len - 2] != 0x0d || data[len - 1] != 0x0a) {
    LOG(INFO) << "Invalid log packet, tail need be 0x0d0a.";
    return false;
  }
  return true;
}
}  // namespace autox::pointview