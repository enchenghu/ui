/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt_replayer.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <iostream>

#include "utils/utils.h"

#include "xrt/proto/pointcloud.pb.h"

#include "xrt/record/record_reader.h"

using namespace std::chrono_literals;

XrtReplayer::XrtReplayer(
    std::shared_ptr<autox::pointview::DisplayContext> context,
    autox::pointview::DeviceBaseParameter& parameter)
    : autox::pointview::DeviceBase(context, parameter) {
  dispatcher_ = XrtMessageDispatcher::getInstance();
  last_open_dirpath_ = QDir::homePath();
  // playback
  device_context_->updatePlayerType(true);
  playback_buffer_ = std::make_shared<PlaybackBuffer>(device_context_);
  playback_buffer_->setFrameCallback(
      [this](std::shared_ptr<autox::xrt::record::RecordMessage> message,
             int idx, double /*t*/) {
        // dispatch message
        dispatcher_->put(message);
        device_context_->updateCurrentFrame(idx);
      });
  // set player callback
  device_context_->registerStartPlayerCb([this]() {
    if (playback_buffer_->getTotalFrames() == 0) {
      QMessageBox::warning(nullptr, "PointView",
                           "no record data, please Open record file first!",
                           QMessageBox::Ok);
      return false;
    }
    playback_buffer_->sync();
    btn_record_file_->setEnabled(false);
    return true;
  });
  device_context_->registerPausePlayerCb([this]() {
    btn_record_file_->setEnabled(true);
    return true;
  });
  // record file
  btn_record_file_ = std::make_shared<QPushButton>();
  btn_record_file_->setText("Open");
  device_context_->addProperty("record file", btn_record_file_);
  connect(btn_record_file_.get(), SIGNAL(clicked(bool)), this,
          SLOT(openRecordClicked()));
  // record file status
  label_record_file_ = std::make_shared<QLabel>();
  label_record_file_->setText("no record file");
  label_record_file_->setAlignment(Qt::AlignRight);
  device_context_->addProperty("record file status", label_record_file_);
  // create thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playing && play_state.is_playback &&
          playback_buffer_->getTotalFrames() > 0) {
        if (play_state.need_playback_jumb) {
          playback_buffer_->jump(play_state.jump_frame_idx);
          playback_buffer_->consumeNextBuffer(false);
        } else {
          playback_buffer_->consumeNextBuffer();
        }
      } else {
        std::this_thread::sleep_for(100ms);
      }
      device_context_->resetPlaybackJump();
    }
  });
}

XrtReplayer::~XrtReplayer() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
}

bool XrtReplayer::parseRecord(const std::string& record_file) {
  // open record
  record_reader_ = std::make_unique<autox::recorder::RecordReader>(record_file);
  if (!record_reader_->IsValid()) {
    return false;
  }
  // progress bar
  QProgressDialog progress("Parse record file...", "Abort Parse", 0, 100,
                           device_context_->getDisplayContext()->getParent());
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setValue(0);
  // parse record loop
  playback_buffer_->clear();
  size_t n = record_reader_->GetHeader().message_number();
  // parse
  size_t cnt = 0;
  while (!record_reader_->ReachEnd() && cnt < n) {
    auto message = std::make_shared<autox::xrt::record::RecordMessage>();
    if (record_reader_->ReadMessage(message.get())) {
      double timestamp = message->time * 1e-9;
      playback_buffer_->addFrame(timestamp, message);
    }
    // update progress
    cnt++;
    if (progress.wasCanceled()) {
      break;
    }
    double v = 1.0 * (cnt + 1) / n;
    progress.setValue((int)(v * 100));
  }
  // update channel list
  std::vector<std::string> lidar_channel_list;
  std::vector<std::string> camera_channel_list;
  for (auto& channel : record_reader_->GetChannelList()) {
    if (record_reader_->GetMessageType(channel) ==
        "autox.drivers.PointCloudPack") {
      lidar_channel_list.push_back(channel);
    } else if (record_reader_->GetMessageType(channel) ==
               "autox.drivers.Image") {
      camera_channel_list.push_back(channel);
    }
  }
  dispatcher_->reset();
  dispatcher_->addLidarChannelList(lidar_channel_list);
  dispatcher_->addCameraChannelList(camera_channel_list);
  // update label_record
  if (playback_buffer_->getTotalFrames() > 0) {
    record_file_ = record_file;
    label_record_file_->setText(QString::fromStdString(record_file));
    std::this_thread::sleep_for(500ms);
  } else {
    record_file_ = "";
    record_reader_->Reset();
    label_record_file_->setText("no record file");
  }
  playback_buffer_->sync();
  device_context_->updateTotalFrame(playback_buffer_->getTotalFrames());
  // sync player
  emit SyncPlayer();
  return true;
}

void XrtReplayer::openRecordClicked() {
  // choose record with dialog
  QString record_file =
      GetOpenFileName("Open Record File", last_open_dirpath_, "All Files(*.*)");
  if (record_file.isNull()) {
    LOG(INFO) << "Do not select a target file.";
    return;
  }
  last_open_dirpath_ = QFileInfo(record_file).dir().absolutePath();
  parseRecord(record_file.toStdString());
}

bool XrtReplayer::initFromConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  // record file
  std::string record_file = "";
  config->getParameter("record_file", record_file);
  // parse record if need
  if (!record_file.empty()) {
    parseRecord(record_file);
  }
  return true;
};

bool XrtReplayer::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  config->setParameter("record_file", record_file_);
  return true;
};
