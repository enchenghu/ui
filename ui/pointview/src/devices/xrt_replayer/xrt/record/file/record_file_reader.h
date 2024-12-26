/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_RECORD_FILE_RECORD_FILE_READER_H_
#define XRT_RECORD_FILE_RECORD_FILE_READER_H_

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <glog/logging.h>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"

#include "xrt/record/file/record_file_base.h"
#include "xrt/record/file/section.h"

namespace autox {
namespace recorder {

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
  bool ReadRecordDebugInfo();
  bool EndOfFile() { return end_of_file_; }

 private:
  bool ReadHeader();
  bool end_of_file_;
};

template <typename T>
bool RecordFileReader::ReadSection(int64_t size, T* message) {
  if (size < INT_MIN || size > INT_MAX) {
    LOG(ERROR) << "Size value greater than the range of int value.";
    return false;
  }
  FileInputStream raw_input(fd_, static_cast<int>(size));
  CodedInputStream coded_input(&raw_input);
  const void* void_pointer;
  int buffer_size;
  coded_input.GetDirectBufferPointerInline(&void_pointer, &buffer_size);
  if (!void_pointer) {
    last_error_ = E_IO_READ;
    return false;
  }
  CodedInputStream::Limit limit = coded_input.PushLimit(static_cast<int>(size));
  if (!message->ParseFromCodedStream(&coded_input)) {
    LOG(ERROR) << "Parse section message failed.";
    end_of_file_ = coded_input.ExpectAtEnd();
    return false;
  }
  if (!coded_input.ConsumedEntireMessage()) {
    LOG(ERROR) << "Do not consumed entire message.";
    return false;
  }
  coded_input.PopLimit(limit);
  if (static_cast<int64_t>(message->ByteSizeLong()) != size) {
    LOG(ERROR) << "Message size is not consistent in section header"
               << ", expect: " << size
               << ", actual: " << message->ByteSizeLong();
    return false;
  }
  return true;
}

}  // namespace recorder
}  // namespace autox

#endif  // XRT_RECORD_FILE_RECORD_FILE_READER_H_
