/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt/record/file/record_file_reader.h"

#include <fcntl.h>

#include <filesystem>

namespace autox {
namespace recorder {

RecordFileReader::RecordFileReader() : end_of_file_(false) {}

RecordFileReader::~RecordFileReader() { Close(); }

bool RecordFileReader::Open(const std::string& path) {
  std::lock_guard<std::mutex> lock(mutex_);
  path_ = path;
  if (!std::filesystem::exists(path_)) {
    LOG(ERROR) << "File not exist, file: " << path_;
    last_error_ = E_NOT_EXIST;
    return false;
  }
  fd_ = open(path_.data(), O_RDONLY);
  if (fd_ < 0) {
    LOG(ERROR) << "Open file failed, file: " << path_ << ", fd: " << fd_
               << ", errno: " << errno;
    last_error_ = E_IO_OPEN;
    return false;
  }
  end_of_file_ = false;
  if (!ReadHeader()) {
    LOG(ERROR) << "Read header section fail, file: " << path_;
    return false;
  }
  if (!ReadRecordDebugInfo()) {
    LOG(ERROR) << "Read recordDebugInfo section fail, file: " << path_;
  }
  return true;
}

void RecordFileReader::Close() {
  if (fd_ >= 0) {
    // when spdk reader close, it may crash, so add 100ms delay
    usleep(100 * 1000);
    close(fd_);
    fd_ = -1;
  }
}

bool RecordFileReader::Reset() {
  if (!SetPosition(sizeof(struct Section) + HEADER_LENGTH)) {
    LOG(ERROR) << "Reset position fail, file: " << path_;
    return false;
  }
  end_of_file_ = false;
  return true;
}

bool RecordFileReader::ReadHeader() {
  Section section;
  if (!ReadSection(&section)) {
    LOG(ERROR)
        << "Read header section fail, file is broken or it is not a record "
           "file.";
    if (last_error_ == NONE) last_error_ = E_FILE_BROKEN;
    return false;
  }
  if (section.type != SectionType::SECTION_HEADER) {
    LOG(ERROR) << "Check section type failed"
               << ", expect: " << SectionType::SECTION_HEADER
               << ", actual: " << section.type;
    last_error_ = E_FILE_BROKEN;
    return false;
  }
  if (!ReadSection<Header>(section.size, &header_)) {
    LOG(ERROR)
        << "Read header section fail, file is broken or it is not a record "
           "file.";
    if (last_error_ == NONE) last_error_ = E_FILE_BROKEN;
    return false;
  }
  if (!SetPosition(sizeof(struct Section) + HEADER_LENGTH)) {
    LOG(ERROR) << "Skip bytes for reaching the nex section failed.";
    last_error_ = E_FILE_BROKEN;
    return false;
  }
  return true;
}

bool RecordFileReader::ReadIndex() {
  if (!header_.is_complete()) {
    LOG(ERROR) << "Record file is not complete.";
    return false;
  }
  if (!SetPosition(header_.index_position())) {
    LOG(ERROR) << "Skip bytes for reaching the index section failed.";
    return false;
  }
  Section section;
  if (!ReadSection(&section)) {
    LOG(ERROR) << "Read index section fail, maybe file is broken.";
    return false;
  }
  if (section.type != SectionType::SECTION_INDEX) {
    LOG(ERROR) << "Check section type failed"
               << ", expect: " << SectionType::SECTION_INDEX
               << ", actual: " << section.type;
    return false;
  }
  if (!ReadSection<Index>(section.size, &index_)) {
    LOG(ERROR) << "Read index section fail.";
    return false;
  }
  Reset();
  return true;
}

bool RecordFileReader::ReadRecordDebugInfo() {
  if (!header_.is_complete()) {
    LOG(ERROR) << "Record file is not complete.";
    return false;
  }
  if (!SetPosition(header_.record_debug_info_position())) {
    LOG(ERROR)
        << "Skip bytes for reaching the record debug info section failed.";
    return false;
  }
  Section section;
  if (!ReadSection(&section)) {
    LOG(ERROR) << "Read record debug info section fail, maybe file is broken.";
    return false;
  }
  if (section.type != SectionType::SECTION_DEBUG_INFO) {
    LOG(ERROR) << "Check section type failed"
               << ", expect: " << SectionType::SECTION_DEBUG_INFO
               << ", actual: " << section.type;
    return false;
  }
  if (!ReadSection<RecordDebugInfo>(section.size, &record_debug_info_)) {
    LOG(ERROR) << "Read record debug info section fail.";
    return false;
  }
  Reset();
  return true;
}

bool RecordFileReader::ReadSection(Section* section) {
  ssize_t count = read(fd_, section, sizeof(struct Section));
  if (count < 0) {
    LOG(ERROR) << "Read fd failed, fd_: " << fd_ << ", errno: " << errno;
    last_error_ = E_IO_READ;
    return false;
  } else if (count == 0) {
    end_of_file_ = true;
    LOG(INFO) << "Reach end of file.";
    return false;
  } else if (count != sizeof(struct Section)) {
    LOG(ERROR) << "Read fd failed, fd_: " << fd_
               << ", expect count: " << sizeof(struct Section)
               << ", actual count: " << count;
    return false;
  }
  return true;
}

bool RecordFileReader::SkipSection(int64_t size) {
  int64_t pos = CurrentPosition();
  if (size > INT64_MAX - pos) {
    LOG(ERROR) << "Current position plus skip count is larger than INT64_MAX, "
               << pos << " + " << size << " > " << INT64_MAX;
    return false;
  }
  if (!SetPosition(pos + size)) {
    LOG(ERROR) << "Skip failed, file: " << path_
               << ", current position: " << pos << "skip count: " << size;
    return false;
  }
  return true;
}

}  // namespace recorder
}  // namespace autox
