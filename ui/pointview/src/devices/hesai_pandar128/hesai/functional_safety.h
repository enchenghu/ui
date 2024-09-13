
/******************************************************************************
 * Copyright 2021 AutoX Inc. All Rights Reserved.
 * Functional safety related methods. For details, see
 * https://drive.google.com/file/d/1gUO2L2NcjBpYLpCBGPq2B6D0uAc6Y15L/view?usp=sharing
 *****************************************************************************/

#pragma once

#include <optional>
#include <string_view>

namespace autox {
namespace drivers {
namespace hesai {

enum class FaultLevel {
  NF = 0,  // No fault, no effect
  FL1,     // Fault level 1, no effect
  FL2,     // Fault level 2, minor harm
  FL3      // Fault level 3, severe harm
};

typedef uint16_t FaultCode;
typedef std::string_view Description;

const std::optional<Description> GetFaultCodeDescription(const FaultCode&);

}  // namespace hesai
}  // namespace drivers
}  // namespace autox
