
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "simple_player_setting.h"

#include <QProgressDialog>

#include "utils.h"

using namespace std::chrono_literals;

namespace autox {
namespace pointview {

SimplePlayerSetting::SimplePlayerSetting(
    std::shared_ptr<DeviceContext> device_context,
    std::shared_ptr<UdpInput> udp_input,
    std::shared_ptr<PcapUdpParser> pcap_parser,
    std::shared_ptr<PlaybackBufferBase> playback_buffer)
    : QObject(device_context->getDisplayContext()->getParent()) {
  device_context_ = device_context;
  udp_input_ = udp_input;
  pcap_parser_ = pcap_parser;
  playback_buffer_ = playback_buffer;
  last_pcap_open_dirpath_ = QDir::homePath();
  last_pcap_memory_open_dirpath_ = QDir::homePath() + "/.pointview/config";
  if (pcap_parser_->readPcapMemory(
          last_pcap_memory_open_dirpath_.toStdString())) {
    LOG(INFO) << "===readPcapMemory init ok";
  } else {
    LOG(ERROR) << "===readPcapMemory init failed";
  };
  // init callback
  reset_driver_cb_ = [](bool /*is_playback*/) {};
  // set player callback
  device_context_->registerPausePlayerCb([this]() {
    combobox_player_type_->setEnabled(true);
    auto play_state = device_context_->getPlayerState();
    edit_udp_port_->setEnabled(!play_state.is_playback);
    btn_pcap_file_->setEnabled(play_state.is_playback);
    btn_mem_pcap_file_->setEnabled(play_state.is_playback);
    if (!play_state.is_playback) {
      udp_input_->close();
    }
    return true;
  });
  device_context_->registerStartRecorderCb(
      [this]() { udp_input_->startRecorder(); });
  device_context_->registerStopRecorderCb(
      [this]() { udp_input_->stopRecorder(); });
  // create property
  auto sub = device_context->getPropertyTree()->createPropertySubTree(
      "Player Setting");
  // combobox_player_type_
  combobox_player_type_ = std::make_shared<QComboBox>();
  combobox_player_type_->addItems({"UDP", "PCAP"});
  connect(combobox_player_type_.get(), SIGNAL(currentIndexChanged(int)), this,
          SLOT(playerTypeChanged(int)));
  sub->addProperty("type", combobox_player_type_);
  edit_udp_port_ = std::make_shared<QLineEdit>();
  edit_udp_port_->setText("0");
  sub->addProperty("udp port", edit_udp_port_);

  label_udp_speed_ = std::make_shared<QLabel>();
  label_udp_speed_->setText("0 kB/s");
  sub->addProperty("udp speed", label_udp_speed_);
  // pcap memory file
  btn_mem_pcap_file_ = std::make_shared<QPushButton>();
  btn_mem_pcap_file_->setText("choose");
  sub->addProperty("pcap mem file", btn_mem_pcap_file_);
  connect(btn_mem_pcap_file_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openPcapMemoryClicked()));
  // pcap file
  btn_pcap_file_ = std::make_shared<QPushButton>();
  btn_pcap_file_->setText("Open");
  sub->addProperty("pcap file", btn_pcap_file_);
  connect(btn_pcap_file_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openPcapClicked()));
  // pcap file status
  label_pcap_file_ = std::make_shared<QLabel>();
  label_pcap_file_->setText("no pcap file");
  label_pcap_file_->setAlignment(Qt::AlignRight);
  sub->addProperty("pcap file status", label_pcap_file_);

  label_packet_index_ = std::make_shared<QLabel>();
  label_packet_index_->setText("begin: none, end: none");
  sub->addProperty("udp index in pcap", label_packet_index_);
  // pcap filter
  auto pcap_filter_sub = sub->createPropertySubTree("pcap filter");
  checkbox_enable_pcap_filter_ = std::make_shared<QCheckBox>();
  pcap_filter_sub->addProperty("enable", checkbox_enable_pcap_filter_);
  edit_pcap_filter_udp_port_ = std::make_shared<QLineEdit>();
  edit_pcap_filter_udp_port_->setText("0");
  pcap_filter_sub->addProperty("dst port", edit_pcap_filter_udp_port_);
  // debug
  auto debug_sub = sub->createPropertySubTree("debug");
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
  spinbox_constant_fps_->setMaximum(30);
  spinbox_constant_fps_->setValue(constant_fps_);
  debug_sub->addProperty("constant fps", spinbox_constant_fps_);
  connect(spinbox_constant_fps_.get(),
          static_cast<void (QDoubleSpinBox::*)(double)>(
              &QDoubleSpinBox::valueChanged),
          [this](double v) {
            constant_fps_ = v;
            playback_buffer_->setConstantFps(constant_fps_);
          });

  btn_record_path_ = std::make_shared<QPushButton>();
  btn_record_path_->setText("set");
  sub->addProperty("record path", btn_record_path_);
  // pcap file status
  connect(btn_record_path_.get(), SIGNAL(clicked(bool)), this,
          SLOT(setRecordPath()));

  label_record_path_ = std::make_shared<QLabel>();
  label_record_path_->setText("no record path");
  label_record_path_->setAlignment(Qt::AlignRight);
  sub->addProperty("record path status", label_record_path_);
  // create thread

  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playback) {
        if (play_state.need_playback_reload) {
          playback_buffer_->initSafeQueue();
          device_context_->resetPlaybackReload();
        }
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
        if (play_state.is_playing) {
          udp_input_->poll();
        } else {
          std::this_thread::sleep_for(100ms);
        }
      }
    }
  });
  pthread_setname_np(player_thread_->native_handle(), "pv/player");
  // update state
  edit_udp_port_->setEnabled(true);
  btn_pcap_file_->setEnabled(false);
  btn_mem_pcap_file_->setEnabled(false);
}

SimplePlayerSetting::~SimplePlayerSetting() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
  if (parse_all_thread_) {
    exit_a = true;
    pcap_parser_->breakParseAllPackets(true);
    if (parse_all_thread_->joinable()) parse_all_thread_->join();
  }
}

void SimplePlayerSetting::setSyncPlayerCb(std::function<void()> cb) {
  sync_player_cb_ = cb;
}

void SimplePlayerSetting::setResetDriverCb(std::function<void(bool)> cb) {
  reset_driver_cb_ = cb;
}

void SimplePlayerSetting::playerTypeChanged(int index) {
  bool is_playback = (index == 1);
  device_context_->updatePlayerType(is_playback);
  edit_udp_port_->setEnabled(!is_playback);
  btn_pcap_file_->setEnabled(is_playback);
  btn_mem_pcap_file_->setEnabled(is_playback);
  if (sync_player_cb_) {
    sync_player_cb_();
  }
}

bool SimplePlayerSetting::parsePcap() {
  if (parse_all_thread_) {
    exit_a = true;
    pcap_parser_->breakParseAllPackets(true);
    if (parse_all_thread_->joinable()) parse_all_thread_->join();
  }
  reset_driver_cb_(true);
  if (set_driver_thread_cb_) set_driver_thread_cb_(0);
  playback_buffer_->clear();
  pcap_parser_->breakParseAllPackets(false);
  parse_all_thread_ = std::make_unique<std::thread>([this]() {
    LOG(INFO) << "======make parse_all_thread_ =====";
    exit_a = false;
    std::unique_lock<std::mutex> lk(read_packets_mutex_);
    read_packets_ok_.wait(lk);
    LOG(INFO) << "======start parse_all_thread_ =====";
    if (set_driver_thread_cb_) set_driver_thread_cb_(driver_thread_num_);
    playback_buffer_->sync();
    while (!exit_a) {
      bool state = pcap_parser_->parseAllPackets();
      if (!state) break;
      std::this_thread::sleep_for(100ms);
    }
    pcap_parser_->breakParseAllPackets(true);
    LOG(INFO) << "======quit parse_all_thread_ =====";
  });

  if (pcap_parser_->getParserState()) {
    std::string pcap_file = pcap_parser_->getPcapFilePath();
    label_pcap_file_->setText(QString::fromStdString(pcap_file));
    playback_buffer_->addRTFrameNum(pcap_parser_->getFrameNum());
    LOG(INFO) << "===init already done before===";
    device_context_->updateTotalFrame(playback_buffer_->getRTFrameNum());
    std::this_thread::sleep_for(100ms);
    read_packets_ok_.notify_one();
    if (sync_player_cb_) {
      sync_player_cb_();
    }
    return playback_buffer_->getRTFrameNum() > 0;
  }
  // progress bar
  QProgressDialog progress("Parse pcap file...", "Abort Parse", 0, 100,
                           device_context_->getDisplayContext()->getParent());
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setValue(0);
  auto progress_cb = [&](double v) {
    if (progress.wasCanceled()) {
      return false;
    }
    progress.setValue((int)(v * 100));
    return true;
  };
  // pcap_parser_->reset();
  if (pcap_parser_->isOpen() && pcap_parser_->parseAllPackets(progress_cb)) {
    std::string pcap_file = pcap_parser_->getPcapFilePath();
    label_pcap_file_->setText(QString::fromStdString(pcap_file));
    LOG(INFO)
        << "=========init done=========: playback_buffer_->getRTFrameNum() is "
        << playback_buffer_->getRTFrameNum();
    device_context_->updateTotalFrame(playback_buffer_->getRTFrameNum());
  } else {
    pcap_parser_->close();
    label_pcap_file_->setText("no pcap file");
    exit_a = true;
    playback_buffer_->clear();
  }
  read_packets_ok_.notify_one();
  if (sync_player_cb_) {
    sync_player_cb_();
  }
  return playback_buffer_->getRTFrameNum() > 0;
}

void SimplePlayerSetting::setRecordPath() {
  QString save_folder_ = QFileDialog::getExistingDirectory(
      device_context_->getDisplayContext()->getParent(),
      "Choose folder for Recording File", QDir::currentPath(),
      QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly |
          QFileDialog::DontResolveSymlinks);

  if (!save_folder_.isNull()) {
    LOG(INFO) << "selected recording folder : " << save_folder_.toStdString();
  } else {
    save_folder_ = QDir::homePath();
    LOG(INFO) << "default recording folder : " << save_folder_.toStdString();
  }
  label_record_path_->setText(save_folder_);
  device_context_->updateRecordPath(save_folder_.toStdString());
}

bool SimplePlayerSetting::openPcapMemoryClicked() {
  QString pcap_memory_folder_ = QFileDialog::getExistingDirectory(
      device_context_->getDisplayContext()->getParent(),
      "Choose folder for Pcap Memory file", last_pcap_memory_open_dirpath_,
      QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly |
          QFileDialog::DontResolveSymlinks);

  last_pcap_memory_open_dirpath_ = pcap_memory_folder_;
  if (!pcap_memory_folder_.isNull()) {
    LOG(INFO) << "selected Pcap Memory file path : "
              << pcap_memory_folder_.toStdString();
    if (pcap_parser_->readPcapMemory(pcap_memory_folder_.toStdString())) {
      return true;
    } else {
      QMessageBox::warning(nullptr, "PointView",
                           "current path doesn't have Pcap Memory file!",
                           QMessageBox::Ok);
    }
  }
  return false;
}

bool SimplePlayerSetting::openPcapClicked() {
  // set filter
  if (checkbox_enable_pcap_filter_->isChecked()) {
    std::string port = edit_pcap_filter_udp_port_->text().toStdString();
    pcap_parser_->setFilter("", stoi(port));
  }
  // choose pcap with dialog
  QString pcap_file = GetOpenFileName("Open Pcap File", last_pcap_open_dirpath_,
                                      "Pcap File(*.pcap);;All Files(*.*)");
  if (pcap_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return false;
  }
  last_pcap_open_dirpath_ = QFileInfo(pcap_file).dir().absolutePath();
  // open file
  if (pcap_parser_->open(pcap_file.toStdString())) {
    return parsePcap();
  }
  return false;
}

void SimplePlayerSetting::refreshUdpSpeed() {
  float speed_m = udp_input_->getUdpSpeed();
  if (speed_m > 1024.0) {
    speed_m /= 1024.0;
    label_udp_speed_->setText(QString::number(speed_m, 'f', 2) + "MB/s");
  } else
    label_udp_speed_->setText(QString::number(speed_m) + "kB/s");
}

void SimplePlayerSetting::updateUdpIndexPcap(size_t begin, size_t end) {
  label_packet_index_->setText("begin: " + QString::number(begin) +
                               ", end: " + QString::number(end));
}

bool SimplePlayerSetting::startPlayer() {
  auto play_state = device_context_->getPlayerState();
  if (play_state.is_playback) {
    if (playback_buffer_->getRTFrameNum() == 0) {
      QMessageBox::warning(nullptr, "PointView",
                           "no pcap data, please Open pcap file first!",
                           QMessageBox::Ok);
      return false;
    }
    playback_buffer_->sync();
  } else {
    reset_driver_cb_(false);
    std::string port = edit_udp_port_->text().toStdString();
    if (port.empty()) {
      return false;
    }
    if (!udp_input_->open(stoi(port))) {
      return false;
    }
  }
  combobox_player_type_->setEnabled(false);
  edit_udp_port_->setEnabled(false);
  btn_pcap_file_->setEnabled(false);
  btn_mem_pcap_file_->setEnabled(false);
  return true;
}

bool SimplePlayerSetting::initFromConfig(std::shared_ptr<Config> config) {
  // get parameter
  bool is_playback = false;
  config->getParameter(namespace_ + ".is_playback", is_playback);
  int type_index = is_playback ? 1 : 0;
  combobox_player_type_->setCurrentIndex(type_index);
  playerTypeChanged(type_index);
  // udp port
  std::string udp_port = "0";
  config->getParameter(namespace_ + ".udp_port", udp_port);
  edit_udp_port_->setText(QString::fromStdString(udp_port));
  // pcap file
  std::string pcap_file = "";
  config->getParameter(namespace_ + ".pcap_file", pcap_file);
  // pcap_filter
  bool enable_pcap_filter = false;
  config->getParameter(namespace_ + ".pcap_filter.enable", enable_pcap_filter);
  checkbox_enable_pcap_filter_->setChecked(enable_pcap_filter);
  std::string dst_port = "";
  config->getParameter(namespace_ + ".pcap_filter.dst_port", dst_port);
  edit_pcap_filter_udp_port_->setText(QString::fromStdString(dst_port));
  // debug
  config->getParameter(namespace_ + ".debug.enable", use_constant_fps_);
  config->getParameter(namespace_ + ".debug.constant_fps", constant_fps_);
  checkbox_enable_constant_fps_->setChecked(use_constant_fps_);
  spinbox_constant_fps_->setValue(constant_fps_);
  // parse pcap if need
  if (is_playback && !pcap_file.empty()) {
    // set filter
    if (checkbox_enable_pcap_filter_->isChecked()) {
      std::string port = edit_pcap_filter_udp_port_->text().toStdString();
      pcap_parser_->setFilter("", stoi(port));
    }
    pcap_parser_->open(pcap_file);
    parsePcap();
  }
  return true;
}

bool SimplePlayerSetting::storeToConfig(std::shared_ptr<Config> config) {
  config->setParameter(namespace_ + ".is_playback",
                       device_context_->getPlayerState().is_playback);
  config->setParameter(namespace_ + ".udp_port",
                       edit_udp_port_->text().toStdString());
  config->setParameter(namespace_ + ".pcap_file",
                       pcap_parser_->getPcapFilePath());
  config->setParameter(namespace_ + ".pcap_filter.enable",
                       checkbox_enable_pcap_filter_->isChecked());
  config->setParameter(namespace_ + ".pcap_filter.dst_port",
                       edit_pcap_filter_udp_port_->text().toStdString());
  config->setParameter(namespace_ + ".debug.enable", use_constant_fps_);
  config->setParameter(namespace_ + ".debug.constant_fps", constant_fps_);
  return true;
}

void SimplePlayerSetting::setUdpPort(std::string port) {
  edit_udp_port_->setText(QString::fromStdString(port));
}

}  // namespace pointview
}  // namespace autox
