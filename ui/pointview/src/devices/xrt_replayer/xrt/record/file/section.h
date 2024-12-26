/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef XRT_RECORD_FILE_SECTION_H_
#define XRT_RECORD_FILE_SECTION_H_

namespace autox {
namespace recorder {

struct Section {
  SectionType type;
  int64_t size;
};

}  // namespace recorder
}  // namespace autox

#endif  // XRT_RECORD_FILE_SECTION_H_
