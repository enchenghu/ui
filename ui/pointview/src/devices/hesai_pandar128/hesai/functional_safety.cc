
/******************************************************************************
 * Copyright 2021 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#include "hesai/functional_safety.h"

#include <unordered_map>

namespace autox {
namespace drivers {
namespace hesai {

// create description based on this msg pattern
#define REGISTER_FAULT_CODE(code, msg) {code, "FaultCode " #code ": " msg}

static const std::unordered_map<FaultCode, Description>
kFaultCodeDescriptionMap{
  // Voltage diagnosis
  REGISTER_FAULT_CODE(0x0012, "Overvoltage/Undervoltage fault"),
  REGISTER_FAULT_CODE(0x0021, "Overvoltage/Undervoltage fault"),
  // Clock diagnosis
  REGISTER_FAULT_CODE(0x0132, "Master clock (125 MHz) fault"),
  REGISTER_FAULT_CODE(0x0112, "Functional clock fault"),
  REGISTER_FAULT_CODE(0x0122, "EWD (external watch dag) self-test fault"),
  REGISTER_FAULT_CODE(0x0142, "Master clock (125 MHz) fault"),
  REGISTER_FAULT_CODE(0x0152, "Functional clock fault"),
  // Program flow diagnosis
  REGISTER_FAULT_CODE(0x0212, "Program flow exception"),
  REGISTER_FAULT_CODE(0x0222, "Program flow exception"),
  // Processsing unit diagnosis
  REGISTER_FAULT_CODE(0x0312, "Processing unit exception"),
  REGISTER_FAULT_CODE(0x0322, "Processing unit exception"),
  // memory diagnosis
  REGISTER_FAULT_CODE(0x0412, "CRC Fault of NOR flash memory"),
  REGISTER_FAULT_CODE(0x0423,
     "Self-test fault of the safety mechanism for configuration memory "
     "diagnosis"),
  REGISTER_FAULT_CODE(0x0421,
     "Correctable single-bit fault in the configuration memory "),
  REGISTER_FAULT_CODE(0x0422,
     "Double-bit fault or uncorrectable single-bit fault in the "
     "configuration memory"),
  REGISTER_FAULT_CODE(0x0425,
     "Communication fault of the safety mechanism for configuration memory "
     "diagnosis"),
  REGISTER_FAULT_CODE(0x0463,
     "Self-test fault in ECC (error correction code) diagnosis for BRAM "
     "(block RAM)"),
  REGISTER_FAULT_CODE(0x0461, "Single-bit fault in BRAM"),
  REGISTER_FAULT_CODE(0x0462, "Double-bit fault in BRAM"),
  REGISTER_FAULT_CODE(0x0442, "CRC fault of NOR flash memory"),
  REGISTER_FAULT_CODE(0x04A1,
     "The xrtsecurity-related data (which needs authenticating) is "
     "abnormal"),
  REGISTER_FAULT_CODE(0x0453,
     "Self-test fault of the safety mechanism for configuration memory "
     "diagnosis"),
  REGISTER_FAULT_CODE(0x0451,
     "Correctable single-bit fault in the configuration memory "),
  REGISTER_FAULT_CODE(0x0452,
     "Double-bit fault or uncorrectable single-bit fault in the "
     "configuration memory"),
  REGISTER_FAULT_CODE(0x0463, "Self-test fault in ECC diagnosis for BRAM"),
  REGISTER_FAULT_CODE(0x0462, "Single-bit fault in BRAM"),
  REGISTER_FAULT_CODE(0x0461, "Double-bit fault in BRAM"),
  REGISTER_FAULT_CODE(0x0473, "Self-test fault in ECC diagnosis for DDR"),
  REGISTER_FAULT_CODE(0x0471, "Single-bit fault in DDR"),
  REGISTER_FAULT_CODE(0x0472, "Double-bit fault in DDR"),
  REGISTER_FAULT_CODE(0x0481, "CRC fault in eMMC"),
  REGISTER_FAULT_CODE(0x04C1, "Register fault"),
  // internal function logi monitor
  REGISTER_FAULT_CODE(0x0611,
     "Laser receiver module exceeds the first-level overvoltage threshold "
     "OR drops below the first level undervoltage threshold"),
  REGISTER_FAULT_CODE(0x0612,
     "Laser receiver module exceeds the second-level overvoltage threshold "
     "OR drops below the second level undervoltage threshold"),
  REGISTER_FAULT_CODE(0x0632,
     "Internal TDC module fault in the distance and reflectivity link"),
  REGISTER_FAULT_CODE(0x0622,
     "Distance and reflectivity calculation module fault in the distance "
     "and reflectivity link"),
  REGISTER_FAULT_CODE(0x0642, "Eye safety module fault"),
  REGISTER_FAULT_CODE(0x0652,
     "Timing controller module fault in the distance and reflectivity "
     "link"),
  REGISTER_FAULT_CODE(0x0662, "Point cloud data transmission link fault"),
  REGISTER_FAULT_CODE(0x0661, "Interstitial point filtering module fault"),
  REGISTER_FAULT_CODE(0x0972, "Azimuth processing module fault"),
  // Point cloud data missing diagnosis by azimuth angle
  REGISTER_FAULT_CODE(0x0682,
     "Point cloud data missing fault: the azimuth spacing between two UDP "
     "packets exceeds the upper threshold"),
  REGISTER_FAULT_CODE(0x0681,
     "Point cloud data missing fault: the azimuth spacing between two UDP "
     "packets drops below the lower threshold"),
  // Motor diagnosis
  REGISTER_FAULT_CODE(0x0711, "Abnormal start of motor"),
  REGISTER_FAULT_CODE(0x0721, "Abnormal motor speed"),
  REGISTER_FAULT_CODE(0x0722, "Motor stall fault"),
  REGISTER_FAULT_CODE(0x0732, "Motor overcurrent fault"),
  REGISTER_FAULT_CODE(0x0742, "Motor rotation direction fault"),
  // Encoder diagnosis
  REGISTER_FAULT_CODE(0x0812,
     "A-phase signal is abnormal in the main encoder or in both encoders"),
  REGISTER_FAULT_CODE(0x0811,
     "B-phase signal is abnormal in the main encoder or the slave "
     "encoders"),
  // Encoder disk contamination diagnosis
  REGISTER_FAULT_CODE(0x0912, "Encoder disk contamination fault"),
  // Laser emitter diagnosis
  REGISTER_FAULT_CODE(0x0A13, "Self-test fault in laser diagnosis"),
  REGISTER_FAULT_CODE(0x0A11, "Single-blind-channel fault"),
  REGISTER_FAULT_CODE(0x0A12, "Multi-blind-channel fault"),
  // Baseline noise diagnosis of received signal
  REGISTER_FAULT_CODE(0x0B11, "Single-channel abnormal baseline noise fault"),
  REGISTER_FAULT_CODE(0x0B12, "Multi-channel abnormal baseline noise fault"),
  // Laser receiver diagnosis
  REGISTER_FAULT_CODE(0x0B24,
      "Abnormal connenction of the diagnostic hardware module"),
  REGISTER_FAULT_CODE(0x0B23, "Self-test fault in laser receiver diagnosis"),
  REGISTER_FAULT_CODE(0x0B21, "Single-blind-channel fault"),
  REGISTER_FAULT_CODE(0x0B22,
      "Multi-blind-channel fault or abnormal laser receiver signal"),
  REGISTER_FAULT_CODE(0x0B34,
      "Unintended start of the safety mechanism in laser receiver "
      "diagnosis"),
  // A/D and D/A conversion diagnosis for photoecletric singal processing
  REGISTER_FAULT_CODE(0x0C11,
     "Abnormal DAC/ADC/TDC module or loss of the point cloud distance "
     "calib signal"),
  REGISTER_FAULT_CODE(0x0C12, "Abnormal point cloud distance calib signal"),
  REGISTER_FAULT_CODE(0x1411,
     "Abnormal voltage and temperature sampling signal"),
  // Temperature diagnosis
  REGISTER_FAULT_CODE(0x0D15,
     "Hardware rationality fault of the temperature sensor"),
  REGISTER_FAULT_CODE(0x0D16, "Cross-check fault of the temperature sensor"),
  REGISTER_FAULT_CODE(0x0D11,
     "Safety_related components exceed the first-level over-temperature "
     "threshold"),
  REGISTER_FAULT_CODE(0x0D12,
     "Safety_related components exceed the second-level over-temperature "
     "threshold"),
  REGISTER_FAULT_CODE(0x0D25,
     "Hardware rationality fault of the temperature sensor"),
  // Uplink wireless communication diagnosis
  REGISTER_FAULT_CODE(0x0E11, "Uplink data CRC fault"),
  // Donwlink wireless communication diagnosis
  REGISTER_FAULT_CODE(0x0E22,
     "Donwlink data stuck fault, or timeout fault, or CRC fault"),
  REGISTER_FAULT_CODE(0x0E32,
     "Abnormal synchronization between bottom and top controllers"),
  // Connector diagnosis
  REGISTER_FAULT_CODE(0x0F12, "Abnormal hardware connection"),
  // Synchronous time diagnosis
  REGISTER_FAULT_CODE(0x1011, "Abnormal PTP synchronization"),
  // Exception recovery of software process diagnosis
  REGISTER_FAULT_CODE(0x1111, "Abnormal restart of software process"),
  // Abnormal boot of software diagnosis
  REGISTER_FAULT_CODE(0x1121, "Abnormal boot of software"),
  // Humidity sensor diagnosis
  REGISTER_FAULT_CODE(0x1211,
     "Hardware rationality fault of the humidity sensor"),
  // ADC diagnosis
  REGISTER_FAULT_CODE(0x1411, "ADC fault of the control-top"),
  // IMU diagnosis
  REGISTER_FAULT_CODE(0x1221,
     "Communication timeout or data reasonableness fault")
};

const std::optional<Description> GetFaultCodeDescription(const FaultCode& fc) {
  const auto iter = kFaultCodeDescriptionMap.find(fc);
  if (iter != kFaultCodeDescriptionMap.end()) {
    return iter->second;
  }
  return {};
}

}  // namespace hesai
}  // namespace drivers
}  // namespace autox
