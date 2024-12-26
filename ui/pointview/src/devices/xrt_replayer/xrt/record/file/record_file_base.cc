/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt/record/file/record_file_base.h"

#include <sys/types.h>
#include <unistd.h>

#include <glog/logging.h>

namespace autox {
namespace recorder {

int64_t RecordFileBase::CurrentPosition() {
  off_t pos = lseek(fd_, 0, SEEK_CUR);
  if (pos < 0) {
    LOG(ERROR) << "lseek failed, file: " << path_ << ", fd: " << fd_
               << ", offset: 0, whence: SEEK_CUR"
               << ", position: " << pos << ", errno: " << errno;
  }
  return pos;
}

bool RecordFileBase::SetPosition(int64_t position) {
  off_t pos = lseek(fd_, position, SEEK_SET);
  if (pos < 0) {
    LOG(ERROR) << "lseek failed, file: " << path_ << ", fd: " << fd_
               << ", offset: 0, whence: SEEK_SET"
               << ", position: " << pos << ", errno: " << errno;
    return false;
  }
  return true;
}

}  // namespace recorder
}  // namespace autox
