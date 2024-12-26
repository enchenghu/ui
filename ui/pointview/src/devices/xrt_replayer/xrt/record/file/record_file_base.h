/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_RECORD_FILE_RECORD_FILE_BASE_H_
#define XRT_RECORD_FILE_RECORD_FILE_BASE_H_

#include <mutex>
#include <string>

#include "xrt/proto/record.pb.h"

namespace autox {
namespace recorder {

const int HEADER_LENGTH = 2048;

using ::autox::xrt::proto::Channel;
using ::autox::xrt::proto::ChannelCache;
using ::autox::xrt::proto::ChannelInfo;
using ::autox::xrt::proto::ChunkBody;
using ::autox::xrt::proto::ChunkBodyCache;
using ::autox::xrt::proto::ChunkHeader;
using ::autox::xrt::proto::ChunkHeaderCache;
using ::autox::xrt::proto::CompressType;
using ::autox::xrt::proto::Header;
using ::autox::xrt::proto::Index;
using ::autox::xrt::proto::RecordDebugInfo;
using ::autox::xrt::proto::SectionType;
using ::autox::xrt::proto::SingleIndex;
using ::autox::xrt::proto::SingleMessage;

class RecordFileBase {
 public:
  enum ErrorEnum { NONE, E_NOT_EXIST, E_IO_OPEN, E_IO_READ, E_FILE_BROKEN };

  RecordFileBase() {}
  virtual ~RecordFileBase() {}
  virtual bool Open(const std::string& path) = 0;
  virtual void Close() = 0;
  const std::string& GetPath() const { return path_; }
  const Header& GetHeader() const { return header_; }
  const Index& GetIndex() const { return index_; }
  const RecordDebugInfo& GetRecordDebugInfo() const {
    return record_debug_info_;
  }
  virtual int64_t CurrentPosition();
  virtual bool SetPosition(int64_t position);
  ErrorEnum GetError() const { return last_error_; }

 protected:
  std::mutex mutex_;
  std::string path_;
  Header header_;
  Index index_;
  RecordDebugInfo record_debug_info_;
  int fd_ = -1;
  ErrorEnum last_error_ = NONE;
};

}  // namespace recorder
}  // namespace autox

#endif  // XRT_RECORD_FILE_RECORD_FILE_BASE_H_
