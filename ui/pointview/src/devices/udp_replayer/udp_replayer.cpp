
#include "udp_replayer.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <iostream>

using namespace std::chrono_literals;

UdpReplayer::UdpReplayer(
    std::shared_ptr<autox::pointview::DisplayContext> context, int device_id,
    const std::string& device_name)
    : autox::pointview::DeviceBase(context, device_id, device_name) {
  last_pcap_open_dirpath_ = QDir::homePath();
  // create udp socket
  udp_sock_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sock_ < 0) {
    std::cout << "[UdpReplayer] create socket failed!" << std::endl;
  }
  // playback
  device_context_->updatePlayerType(true);
  pcap_parser_ =
      std::make_shared<autox::pointview::PcapUdpParser>(device_context_);
  playback_buffer_ = std::make_shared<PlaybackBuffer>(device_context_);
  playback_buffer_->setFrameCallback(
      [this](std::shared_ptr<UdpPacket> pkt, int idx, double /*t*/) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(pkt->port);
        sendto(udp_sock_, pkt->data.data(), pkt->len, 0,
               reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        device_context_->updateCurrentFrame(idx);
      });
  // set player callback
  device_context_->registerStartPlayerCb([this]() {
    if (playback_buffer_->getRTFrameNum() == 0) {
      QMessageBox::warning(nullptr, "PointView",
                           "no pcap data, please Open pcap file first!",
                           QMessageBox::Ok);
      return false;
    }
    lidar_type_fps_->setEnabled(false);
    playback_buffer_->sync();
    btn_pcap_file_->setEnabled(false);
    return true;
  });
  device_context_->registerPausePlayerCb([this]() {
    btn_pcap_file_->setEnabled(true);
    lidar_type_fps_->setEnabled(true);
    return true;
  });
  // pcap file
  btn_pcap_file_ = std::make_shared<QPushButton>();
  btn_pcap_file_->setText("Open");
  device_context_->addProperty("pcap file", btn_pcap_file_);
  connect(btn_pcap_file_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openPcapClicked()));
  // pcap file status
  label_pcap_file_ = std::make_shared<QLabel>();
  label_pcap_file_->setText("no pcap file");
  label_pcap_file_->setAlignment(Qt::AlignRight);
  device_context_->addProperty("pcap file status", label_pcap_file_);
  // debug
  auto debug_sub =
      device_context_->getPropertyTree()->createPropertySubTree("debug");
  checkbox_enable_constant_fps_ = std::make_shared<QCheckBox>();
  debug_sub->addProperty("enable", checkbox_enable_constant_fps_);
  connect(checkbox_enable_constant_fps_.get(), &QCheckBox::stateChanged,
          [this](int state) {
            use_constant_fps_ = state > 0;
            if (use_constant_fps_) {
              playback_buffer_->setConstantFps(constant_fps_);
            }
            playback_buffer_->enableConstantFps(use_constant_fps_);
          });
  spinbox_constant_fps_ = std::make_shared<QDoubleSpinBox>();
  spinbox_constant_fps_->setMaximum(210000);
  spinbox_constant_fps_->setValue(constant_fps_);
  debug_sub->addProperty("constant fps", spinbox_constant_fps_);
  connect(spinbox_constant_fps_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            constant_fps_ = v;
            playback_buffer_->setConstantFps(constant_fps_);
          });
  lidar_type_fps_ = std::make_shared<QComboBox>();
  lidar_type_fps_->addItem("Xlidar");
  lidar_type_fps_->addItem("Blidar");
  debug_sub->addProperty("lidar type", lidar_type_fps_);
  connect(
      lidar_type_fps_.get(),
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      [this](int index) {
        Debug(std::to_string(index));
        if (index == 0) {
          spinbox_constant_fps_->setValue(200000);
        } else if (index == 1) {
          spinbox_constant_fps_->setValue(9000);
        }
      });
  lidar_type_fps_->setCurrentIndex(1);
  // create thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playback) {
        if (play_state.is_playing && playback_buffer_->getRTFrameNum() > 0) {
          playback_buffer_->consumeNextRTBuffer(true, false);
        } else {
          if (play_state.need_playback_jumb) {
            playback_buffer_->initSafeQueue();
            playback_buffer_->jump(play_state.jump_frame_idx);
            playback_buffer_->consumeNextRTBuffer(true, true);
            device_context_->resetPlaybackJump();
            device_context_->updateJumpRate(0.0);
          }
          std::this_thread::sleep_for(100ms);
        }
      } else {
        std::this_thread::sleep_for(100ms);
      }
    }
  });
}

UdpReplayer::~UdpReplayer() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  if (parse_all_thread_) {
    exit_parse_ = true;
    if (parse_all_thread_->joinable()) parse_all_thread_->join();
  }
}

bool UdpReplayer::parsePcap(const std::string& pcap_file) {
  // open pcap
  if (!pcap_parser_->open(pcap_file)) {
    return false;
  }
  // progress bar
  if (parse_all_thread_) {
    exit_parse_ = true;
    if (parse_all_thread_->joinable()) parse_all_thread_->join();
  }
  exit_parse_ = false;
  QProgressDialog progress("Parse pcap file...", "Abort Parse", 0, 100,
                           device_context_->getDisplayContext()->getParent());
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setValue(0);
  // parse pcap loop
  playback_buffer_->clear();
  packet_cnt_ = pcap_parser_->getPacketNumber();
  std::cout << "stat parse pcap, the number of packets: " << packet_cnt_
            << std::endl;
  uint8_t data[65536];
  size_t len = 0;
  double timestamp;
  int port;
  size_t cnt = 0;
  pcap_parser_->reset();
  pcap_parser_->setFileOffsetMap(0);
  long index_real = 0;
  while (cnt < packet_cnt_) {
    if (pcap_parser_->getPacket(data, &len, &timestamp, &port)) {
      playback_buffer_->addRTFrameNum();
      pcap_parser_->setFileOffsetMap(index_real + 1);
      index_real++;
    }
    // update progress
    if (progress.wasCanceled()) {
      break;
    }
    double v = 1.0 * (cnt + 1) / packet_cnt_;
    progress.setValue((int)(v * 100));
    cnt++;
  }
  pcap_parser_->reset();

  parse_all_thread_ = std::make_unique<std::thread>([this]() {
    std::unique_lock<std::mutex> lk(read_packets_mutex_);
    read_packets_ok_.wait(lk);
    Debug("start parsing packets loop");
    while (!exit_parse_) {
      uint8_t data[65536];
      size_t len = 0;
      double timestamp;
      int port;
      size_t cnt = 0;
      bool reset_jump = false;
      long index_real = 0;
      while (cnt < packet_cnt_ && !exit_parse_) {
        auto state = device_context_->getPlayerState();
        if (!state.need_playback_jumb) reset_jump = false;
        if (!state.is_playing && !state.need_playback_jumb) {
          std::this_thread::sleep_for(50ms);
          continue;
        }
        if (state.need_playback_jumb && !reset_jump) {
          auto jump_frame_idx = state.jump_frame_idx;
          cnt = jump_frame_idx;
          if (cnt < 0) break;
          int ret = pcap_parser_->findCntInMap(jump_frame_idx);
          if (ret == -1) break;
          reset_jump = true;
        }
        if (pcap_parser_->getPacket(data, &len, &timestamp, &port)) {
          auto packet = std::make_shared<UdpPacket>();
          packet->data.resize(len);
          memcpy(packet->data.data(), data, len);
          packet->len = len;
          packet->port = port;
          if (state.need_playback_jumb) index_real = state.jump_frame_idx;
          playback_buffer_->addFrame(index_real++, timestamp, packet);
        } else {
          if (pcap_parser_->eof()) break;
        }
        cnt++;
        if (cnt == packet_cnt_) pcap_parser_->reset();
      }
      pcap_parser_->reset();
    }
    Debug("quit parsing packets loop");
  });
  // update label_pcap
  if (cnt == packet_cnt_) {
    label_pcap_file_->setText(QString::fromStdString(pcap_file));
    std::this_thread::sleep_for(500ms);
  } else {
    pcap_parser_->close();
    label_pcap_file_->setText("no pcap file");
    exit_parse_ = true;
  }
  read_packets_ok_.notify_one();
  playback_buffer_->sync();
  device_context_->updateTotalFrame(playback_buffer_->getRTFrameNum());
  // sync player
  emit SyncPlayer();
  return true;
}

void UdpReplayer::openPcapClicked() {
  // choose pcap with dialog
  QString pcap_file = GetOpenFileName("Open Pcap File", last_pcap_open_dirpath_,
                                      "Pcap File(*.pcap);;All Files(*.*)");
  if (pcap_file.isNull()) {
    Debug("Do not select a target file.");
    return;
  }
  last_pcap_open_dirpath_ = QFileInfo(pcap_file).dir().absolutePath();
  parsePcap(pcap_file.toStdString());
}

bool UdpReplayer::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  // pcap file
  std::string pcap_file = "";
  config->getParameter("pcap_file", pcap_file);
  // parse pcap if need
  if (!pcap_file.empty()) {
    parsePcap(pcap_file);
  }
  return true;
};

bool UdpReplayer::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter("pcap_file", pcap_parser_->getPcapFilePath());
  return true;
};
