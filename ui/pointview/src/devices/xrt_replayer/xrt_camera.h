/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_CAMEAR_H
#define XRT_CAMEAR_H

#include <QLineEdit>
#include <QPushButton>
#include <thread>

#include "utils/common/device_base.h"
#include "utils/frame_info.h"
#include "utils/lidar/range_image.h"

#include "xrt_message_dispatcher.h"

namespace autox {
namespace drivers {
class Image;
}  // namespace drivers
}  // namespace autox

class XrtCamera : public autox::pointview::DeviceBase {
  Q_OBJECT

 public:
  using RawImage = autox::drivers::Image;
  XrtCamera(std::shared_ptr<autox::pointview::DisplayContext> context,
            int device_id, const std::string& device_name);
  ~XrtCamera();
  bool updateUI() override;
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 private:
  bool convertImage(const std::shared_ptr<RawImage>& raw_image, QImage& image);
  bool pausePlayer();

 private:
  // Ui object
  std::shared_ptr<QLineEdit> edit_channel_name_;
  std::shared_ptr<QPushButton> btn_select_channel_;
  // helper utils
  std::shared_ptr<autox::pointview::FrameInfo> info_;
  std::shared_ptr<autox::pointview::RangeImage> image_viewer_;
  // xrt channel dispatcher
  XrtMessageDispatcher* dispatcher_;
  std::string channel_name_{"/xurban/sensor/camera/image"};
  // thread
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
  // raw data image buffer
  std::deque<std::shared_ptr<RawImage>> raw_image_buffer_;
  std::mutex raw_image_mutex_;
  // current frame
  std::shared_ptr<RawImage> current_raw_image_;
  QImage current_image_;
};

#endif  // XRT_CAMEAR_H
