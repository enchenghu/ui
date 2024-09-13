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

#ifndef XRT_RECORD_FILE_RECORD_FILE_READER_H_
#define XRT_RECORD_FILE_RECORD_FILE_READER_H_

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"

#include "xrt/record/file/record_file_base.h"
#include "xrt/record/file/section.h"

namespace autox {
namespace xrt {
namespace record {

using google::protobuf::io::CodedInputStream;
using google::protobuf::io::FileInputStream;
using google::protobuf::io::ZeroCopyInputStream;

class RecordFileReader : public RecordFileBase {
 public:
  RecordFileReader();
  virtual ~RecordFileReader();
  bool Open(const std::string& path) override;
  void Close() override;
  bool Reset();
  bool ReadSection(Section* section);
  bool SkipSection(int64_t size);
  template <typename T>
  bool ReadSection(int64_t size, T* message);
  bool ReadIndex();
  bool EndOfFile() { return end_of_file_; }

 private:
  bool ReadHeader();
  bool end_of_file_;
};

template <typename T>
bool RecordFileReader::ReadSection(int64_t size, T* message) {
  if (size < INT_MIN || size > INT_MAX) {
    std::cout << "Size value greater than the range of int value.";
    return false;
  }
  FileInputStream raw_input(fd_, static_cast<int>(size));
  CodedInputStream coded_input(&raw_input);
  CodedInputStream::Limit limit = coded_input.PushLimit(static_cast<int>(size));
  if (!message->ParseFromCodedStream(&coded_input)) {
    std::cout << "Parse section message failed.";
    end_of_file_ = coded_input.ExpectAtEnd();
    return false;
  }
  if (!coded_input.ConsumedEntireMessage()) {
    std::cout << "Do not consumed entire message.";
    return false;
  }
  coded_input.PopLimit(limit);
  if (static_cast<int64_t>(message->ByteSizeLong()) != size) {
    std::cout << "Message size is not consistent in section header"
           << ", expect: " << size << ", actual: " << message->ByteSizeLong();
    return false;
  }
  return true;
}

}  // namespace record
}  // namespace xrt
}  // namespace autox

#endif  // XRT_RECORD_FILE_RECORD_FILE_READER_H_
