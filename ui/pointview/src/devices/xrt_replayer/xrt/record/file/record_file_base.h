/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef XRT_RECORD_FILE_RECORD_FILE_BASE_H_
#define XRT_RECORD_FILE_RECORD_FILE_BASE_H_

#include <mutex>
#include <string>

#include "xrt/proto/record.pb.h"

namespace autox {
namespace xrt {
namespace record {

const int HEADER_LENGTH = 2048;

using ::autox::xrt::proto::Channel;
using ::autox::xrt::proto::ChannelCache;
using ::autox::xrt::proto::ChunkBody;
using ::autox::xrt::proto::ChunkBodyCache;
using ::autox::xrt::proto::ChunkHeader;
using ::autox::xrt::proto::ChunkHeaderCache;
using ::autox::xrt::proto::CompressType;
using ::autox::xrt::proto::Header;
using ::autox::xrt::proto::Index;
using ::autox::xrt::proto::SectionType;
using ::autox::xrt::proto::SingleIndex;
using ::autox::xrt::proto::SingleMessage;

class RecordFileBase {
 public:
  RecordFileBase() {}
  virtual ~RecordFileBase() {}
  virtual bool Open(const std::string& path) = 0;
  virtual void Close() = 0;
  const std::string& GetPath() const { return path_; }
  const Header& GetHeader() const { return header_; }
  const Index& GetIndex() const { return index_; }
  int64_t CurrentPosition();
  bool SetPosition(int64_t position);

 protected:
  std::mutex mutex_;
  std::string path_;
  Header header_;
  Index index_;
  int fd_ = -1;
};

}  // namespace record
}  // namespace xrt
}  // namespace autox

#endif  // XRT_RECORD_FILE_RECORD_FILE_BASE_H_
