
/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt_camera.h"

#include <QInputDialog>
#include <QMessageBox>
#include <iostream>

#include "xrt/proto/sensor_image.pb.h"

#include "xrt/record/record_message.h"

using namespace std::chrono_literals;

XrtCamera::XrtCamera(std::shared_ptr<autox::pointview::DisplayContext> context,
                     int device_id, const std::string& device_name)
    : autox::pointview::DeviceBase(context, device_id, device_name) {
  // xrt message dispatcher
  dispatcher_ = XrtMessageDispatcher::getInstance();
  // player
  device_context_->registerStartPlayerCb([this]() {
    channel_name_ = edit_channel_name_->text().toStdString();
    edit_channel_name_->setEnabled(false);
    btn_select_channel_->setEnabled(false);
    return true;
  });
  device_context_->registerPausePlayerCb([this]() {
    edit_channel_name_->setEnabled(true);
    btn_select_channel_->setEnabled(true);
    return true;
  });
  // point cloud info
  info_ = std::make_shared<autox::pointview::FrameInfo>(device_context_);
  image_viewer_ =
      std::make_shared<autox::pointview::RangeImage>(device_context_, 640, 480);
  // channel setting
  auto sub = device_context_->getPropertyTree()->createPropertySubTree(
      "channel setting");
  edit_channel_name_ = std::make_shared<QLineEdit>();
  edit_channel_name_->setText(QString::fromStdString(channel_name_));
  sub->addProperty("name", edit_channel_name_);
  btn_select_channel_ = std::make_shared<QPushButton>();
  btn_select_channel_->setText("Open");
  connect(btn_select_channel_.get(), &QPushButton::clicked, [this]() {
    // get lidar channel list
    QStringList channel_list;
    for (auto& channel : dispatcher_->getCameraChannelList()) {
      channel_list << QString::fromStdString(channel);
    }
    if (channel_list.size() == 0) {
      QMessageBox::warning(
          device_context_->getDisplayContext()->getParent(), "PointView",
          "no xrt channel for camera, please check XrrReplayer!",
          QMessageBox::Ok);
      return;
    }
    // open dialog
    bool ok;
    QString selected_channel =
        QInputDialog::getItem(device_context_->getDisplayContext()->getParent(),
                              tr("Select Xrt Channel"), tr("Channel name:"),
                              channel_list, 0, false, &ok);
    if (!ok || selected_channel.isEmpty()) {
      return;
    }
    //
    edit_channel_name_->setText(selected_channel);
  });
  sub->addProperty("selection", btn_select_channel_);
  // create thread
  player_thread_ = std::make_unique<std::thread>([this]() {
    while (!exit_) {
      auto play_state = device_context_->getPlayerState();
      if (play_state.is_playing && !play_state.is_playback) {
        // get message from xrt buffer
        auto message = dispatcher_->get(channel_name_);
        if (message) {
          auto raw_image = std::make_shared<RawImage>();
          raw_image->ParseFromString(message->content);
          std::lock_guard<std::mutex> lock(raw_image_mutex_);
          raw_image_buffer_.push_back(raw_image);
          // publish msg
          if (raw_image_buffer_.size() > 3) {
            raw_image_buffer_.pop_front();
            std::cout << "rendering too long, drop a raw image frame!"
                      << std::endl;
          }
        } else {
          std::this_thread::sleep_for(10ms);
        }
      } else {
        std::this_thread::sleep_for(100ms);
      }
    }
  });
}

XrtCamera::~XrtCamera() {
  exit_ = true;
  if (player_thread_->joinable()) {
    player_thread_->join();
  }
}

bool XrtCamera::convertImage(const std::shared_ptr<RawImage>& raw_image,
                             QImage& image) {
  if (raw_image->encoding() == std::string("rgb8")) {
    assert(raw_image->step() == raw_image->width() * 3);
    int image_width = raw_image->width();
    int image_height = raw_image->height();
    image = QImage(image_width, image_height, QImage::Format_RGB32);
    auto data = raw_image->data().c_str();
    for (int y = 0; y < image.height(); ++y) {
      QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(y));
      for (int x = 0; x < image.width(); ++x) {
        int idx = y * image_width * 3 + x * 3;
        line[x] = qRgb(data[idx], data[idx + 1], data[idx + 2]);
      }
    }
  } else {
    std::cerr << "Cannot support this format (" << raw_image->encoding()
              << ") image" << std::endl;
    return false;
  }
  return true;
}

bool XrtCamera::updateUI() {
  {
    std::lock_guard<std::mutex> lock(raw_image_mutex_);
    if (!raw_image_buffer_.empty()) {
      current_raw_image_ = raw_image_buffer_.front();
      raw_image_buffer_.pop_front();
      device_context_->refreshPointCloud();
    }
  }
  if (!device_context_->getRefreshState()) {
    return false;
  }
  if (!current_raw_image_) {
    return false;
  }
  // assert
  // assert(raw_image->height() == raw_image->height());
  // assert(raw_image->x_size() == raw_image->z_size());
  auto start = std::chrono::steady_clock::now();
  //
  if (convertImage(current_raw_image_, current_image_)) {
    image_viewer_->Update(current_image_);
  }
  // set info
  info_->update(current_raw_image_->measurement_time(),
                current_raw_image_->height() * current_raw_image_->width());
  // update flag
  device_context_->resetRefreshState();
  auto end = std::chrono::steady_clock::now();
  // debug info
  auto update_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  std::cout << "image size: " << current_raw_image_->width() << "x"
            << current_raw_image_->height() << "x"
            << current_raw_image_->step() / current_raw_image_->width()
            << ", encoding:" << current_raw_image_->encoding()
            << ", update time:" << update_time << std::endl;
  return true;
}

bool XrtCamera::initFromConfig(std::shared_ptr<autox::pointview::Config>
                                   config) {  // initFromConfig config
  if (!config) {
    return false;
  }
  // channel_name
  std::string channel_name = "";
  config->getParameter("channel_name", channel_name);
  if (!channel_name.empty()) {
    edit_channel_name_->setText(QString::fromStdString(channel_name));
  }
  return true;
}

bool XrtCamera::storeToConfig(
    std::shared_ptr<autox::pointview::Config> config) {
  // channel_name
  auto channel_name = edit_channel_name_->text().toStdString();
  config->setParameter("channel_name", channel_name);
  return true;
}
