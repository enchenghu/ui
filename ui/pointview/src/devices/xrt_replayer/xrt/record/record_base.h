/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/
#pragma once
#include <stdint.h>

#include <set>
#include <string>

#include "xrt/proto/record.pb.h"

namespace autox {
namespace recorder {

static const std::string& kEmptyString = "";

/**
 * @brief Base class for record reader and writer.
 */
class RecordBase {
 public:
  /**
   * @brief Destructor.
   */
  virtual ~RecordBase() = default;

  /**
   * @brief Get message number by channel name.
   *
   * @param channel_name
   *
   * @return Message number.
   */
  virtual uint64_t GetMessageNumber(const std::string& channel_name) const = 0;

  /**
   * @brief Get message type by channel name.
   *
   * @param channel_name
   *
   * @return Message type.
   */
  virtual const std::string& GetMessageType(
      const std::string& channel_name) const = 0;

  /**
   * @brief Get proto descriptor string by channel name.
   *
   * @param channel_name
   *
   * @return Proto descriptor string by channel name.
   */
  virtual const std::string& GetProtoDesc(
      const std::string& channel_name) const = 0;

  /**
   * @brief Get channel list.
   *
   * @return List container with all channel name string.
   */
  virtual std::set<std::string> GetChannelList() const = 0;

  /**
   * @brief Get record header.
   *
   * @return Record header.
   */
  const autox::xrt::proto::Header& GetHeader() const { return header_; }

  /**
   * @brief Get record file path.
   *
   * @return Record file path.
   */
  const std::string GetFile() const { return file_; }

 protected:
  std::string file_;
  autox::xrt::proto::Header header_;
  bool is_opened_ = false;
};

}  // namespace recorder
}  // namespace autox
