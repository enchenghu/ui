/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_REPLAYER_H
#define XRT_REPLAYER_H

#include <QLabel>
#include <QPushButton>

#include "utils/common/device_base.h"
#include "utils/io/playback_buffer.h"

#include "xrt_message_dispatcher.h"
namespace autox {
namespace recorder {
class RecordReader;
}  // namespace recorder
}  // namespace autox

namespace autox {
namespace xrt {
namespace record {
class RecordMessage;
}
}  // namespace xrt
}  // namespace autox

class XrtReplayer : public autox::pointview::DeviceBase {
  Q_OBJECT
 public:
  using PlaybackBuffer =
      autox::pointview::PlaybackBuffer<autox::xrt::record::RecordMessage>;
  XrtReplayer(std::shared_ptr<autox::pointview::DisplayContext> context,
              autox::pointview::DeviceBaseParameter& parameter);
  ~XrtReplayer() override;
  bool updateUI() override { return true; };
  bool initFromConfig(
      std::shared_ptr<autox::pointview::Config> config) override;
  bool storeToConfig(std::shared_ptr<autox::pointview::Config> config) override;

 private slots:
  void openRecordClicked();

 private:
  bool parseRecord(const std::string& record_file);

 private:
  // Ui object
  std::shared_ptr<QPushButton> btn_record_file_;
  std::shared_ptr<QLabel> label_record_file_;
  // for xrd record playback
  std::string record_file_;
  std::unique_ptr<autox::recorder::RecordReader> record_reader_;
  std::shared_ptr<PlaybackBuffer> playback_buffer_;
  // xrt channel dispatcher
  XrtMessageDispatcher* dispatcher_;
  // thread
  std::unique_ptr<std::thread> player_thread_;
  bool exit_{false};
  //
  QString last_open_dirpath_;
};

#endif  // XRT_REPLAYER_H
