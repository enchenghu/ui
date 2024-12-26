/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/
#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "xrt/proto/record.pb.h"

#include "xrt/record/file/record_file_reader.h"
#include "xrt/record/record_base.h"
#include "xrt/record/record_message.h"

namespace autox {
namespace recorder {

/**
 * @brief The record reader.
 */
class RecordReader : public RecordBase {
 public:
  using FileReaderPtr = std::unique_ptr<RecordFileReader>;
  using ChannelInfoMap =
      std::unordered_map<std::string, autox::xrt::proto::ChannelCache>;

  enum ErrorEnum {
    NONE,
    E_NOT_EXIST,
    E_IO,
    E_FILE_BROKEN,
    E_TIME_OUT_OF_HEADER,
    E_END_TIME_REACHED,
    E_NO_CHUNK_TO_READ
  };

  /**
   * @brief The constructor with record file path as parameter.
   *
   * @param file
   */
  explicit RecordReader(const std::string& file);

  /**
   * @brief The destructor.
   */
  virtual ~RecordReader();

  /**
   * @brief Is this record reader is valid.
   *
   * @return True for valid, false for not.
   */
  bool IsValid() const { return is_valid_; }

  /**
   * @brief Read one message from reader.
   *
   * @param message
   * @param begin_time
   * @param end_time
   *
   * @return True for success, flase for not.
   */
  bool ReadMessage(autox::xrt::record::RecordMessage* message,
                   uint64_t begin_time = 0, uint64_t end_time = UINT64_MAX);

  /**
   * @brief Read one message from reader.
   *
   * @param error detailed error when return false
   * @param message
   * @param begin_time
   * @param end_time
   *
   * @return True for success, flase for not.
   */
  bool ReadMessage(ErrorEnum* error, autox::xrt::record::RecordMessage* message,
                   uint64_t begin_time = 0, uint64_t end_time = UINT64_MAX);

  /**
   * @brief Return detailed error of last read message.
   *
   * @return error enum.
   */
  ErrorEnum GetError() const { return last_error_; }

  /**
   * @brief Reset the message index of record reader.
   */
  void Reset();

  /**
   * @brief Get message number by channel name.
   *
   * @param channel_name
   *
   * @return Message number.
   */
  uint64_t GetMessageNumber(const std::string& channel_name) const override;

  /**
   * @brief Get message type by channel name.
   *
   * @param channel_name
   *
   * @return Message type.
   */
  const std::string& GetMessageType(
      const std::string& channel_name) const override;

  /**
   * @brief Get proto descriptor string by channel name.
   *
   * @param channel_name
   *
   * @return Proto descriptor string by channel name.
   */
  const std::string& GetProtoDesc(
      const std::string& channel_name) const override;

  /**
   * @brief Get channel list.
   *
   * @return List container with all channel name string.
   */
  std::set<std::string> GetChannelList() const override;

  /**
   * @brief Is this record reader is read completed.
   *
   * @return True for reach end, false for not.
   */
  bool ReachEnd() const { return reach_end_; }

  /**
   * @brief Get debug info.
   *
   * @return proto with debug info.
   */
  autox::xrt::proto::BagFileDebugInfo GetDebugInfo();

  /**
   * @brief Get the Index by traversing the file
   */
  void RecoverIndex();

 private:
  bool ReadNextChunk(uint64_t begin_time, uint64_t end_time);

  bool is_valid_ = false;
  bool reach_end_ = false;
  std::unique_ptr<autox::xrt::proto::ChunkBody> chunk_ = nullptr;
  autox::xrt::proto::Index index_;
  autox::xrt::proto::RecordDebugInfo record_debug_info_;
  int message_index_ = 0;
  ChannelInfoMap channel_info_;
  FileReaderPtr file_reader_;
  ErrorEnum last_error_ = NONE;
};

}  // namespace recorder
}  // namespace autox
