/******************************************************************************
 * Copyright 2021 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#include "hesai/pandar128.h"

#include <sys/types.h>

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include "hesai/laser_ts.h"

#ifndef GLOBAL_CONFIG_DIR
#define GLOBAL_CONFIG_DIR "../config"
#endif

namespace autox {
namespace drivers {
namespace hesai {

std::array<double, kRotationMaxUnits> Pandar128Driver::sin_lookup_table_;
std::array<double, kRotationMaxUnits> Pandar128Driver::cos_lookup_table_;

constexpr int kHsLidarL128BlockNumber = 2;
// each block have  128 Unit
constexpr int kHsLidarL128UnitNum = 128;

#pragma pack(push, 1)  // force no struct padding
typedef struct Pandar128Header_s {
  uint16_t sop;
  uint8_t protocalVersionMajor;
  uint8_t protocalVersionMinor;
  uint16_t reserved;

  uint8_t chLaserNumber;
  uint8_t chBlockNumber;
  uint8_t echoCount;

  uint8_t chDisUnit;
  uint8_t echoNum;  // 0x01: single return ; 0x02: dual return
  uint8_t flags;
} Pandar128Header;

typedef struct Pandar128Unit_s {
  uint16_t distance;
  uint8_t reflectivity;
} Pandar128Unit;

typedef struct Pandar128Block_s {
  uint16_t azimuth;  // packet angle  ,Azimuth = RealAzimuth * 100
  Pandar128Unit units[kHsLidarL128UnitNum];
} Pandar128Block;

typedef struct LidarStateInfo_s {
  enum State {
    INIT = 0,
    NORMAL,
    WARN,
    PRE_PERF_DEGRADATION,
    PERF_DEGRADATION,
    PRE_SHUTDOWN,
    SHUTDOWN,
    STANDBY_MODE
  };
  // [7:5] is the LiDAR's current state
  State LidarState() const { return static_cast<State>(data >> 5); }
  // [4:3] is the type of the fault code in this data packet
  // b-01: current fault; b-10: past fault
  bool IsCurrentFault() const { return (data & (1 << 3)); }
  // [2:0] indicates whether the fault reporting system gets stuck
  uint8_t RollingCounter() const { return data & ((1 << 3) - 1); }

 private:
  uint8_t data;
} LidarStateInfo;
typedef struct FaultCodeStatistics_s {
  // [3:0] is the seq number of the fault code in this queue, starting from 1
  uint8_t FaultCodeID() const { return data & ((1 << 4) - 1); }
  // [7:4] counts the total number of fault codes in this queue
  uint8_t TotalFaultCodeNum() const { return data >> 4; }

 private:
  uint8_t data;
} FaultCodeStatistics;
typedef struct Pandar128FunctionalSafetyInfo_s {  // 17 bytes
  uint8_t FSVersion;                              // currently 0x00
  LidarStateInfo lidarStateInfo;
  FaultCodeStatistics faultCodeStatistics;
  FaultCode faultCode;
  uint8_t reserved[8];
  uint32_t CRC2;
} Pandar128FunctionalSafetyInfo;

struct Pandar128Packet_s {
  Pandar128Header header;
  Pandar128Block blocks[kHsLidarL128BlockNumber];
  unsigned int block_crc;

  // functional safety
  Pandar128FunctionalSafetyInfo functional_safety;

  // tail
  uint8_t reserved[9];
  uint16_t azimuth_flag;
  uint8_t operational_state;
  uint8_t return_mode;
  uint16_t motor_speed;
  uint8_t addtime[6];
  uint32_t timestamp;

  uint8_t factoryInfo;  // 0x42
  uint32_t udpSequence;
  uint16_t imuTemperature;       // unit 0.01 Celsius
  uint16_t imuAccelerationUnit;  // * 0.001mg
  uint16_t imuAngularVelUnit;    // * 0.01 milli degree per second
  uint32_t imuTimestamp;         // unit 25 us, starting from powering up
  uint16_t imuXAxisAcceleration;
  uint16_t imuYAxisAcceleration;
  uint16_t imuZAxisAcceleration;
  uint16_t imuXAngularVelocity;
  uint16_t imuYAngularVelocity;
  uint16_t imuZAngularVelocity;
  uint32_t CRC3;
};
#pragma pack(pop)

// packet xrt security size
constexpr int kHsLidarL128XrtSecuritySize = 32;

// packet size without optional Xrt security info : 861
constexpr int kHsLidarL128_PacketSize = sizeof(Pandar128Packet);

// Packet size with Xrt security info: 893
constexpr int kHsLidarL128_CSC_PacketSize =
    kHsLidarL128_PacketSize + kHsLidarL128XrtSecuritySize;

static const float hs128_elev_angle_map[] = {
    14.436,     13.535,     13.081786,  12.624,     12.165246,  11.702,
    11.238522,  10.771,     10.305007,  9.83,       9.356123,   8.88,
    8.401321,   7.921,      7.43808,    6.952581,   6.466905,   5.977753,
    5.487,      4.995801,   4.501,      4.007293,   3.509,      3.012822,
    2.512,      2.013,      1.885,      1.761,      1.637,      1.511,
    1.385875,   1.2582,     1.13,       1.008459,   0.88,       0.756,
    0.63,       0.505,      0.378591,   0.251,      0.124,      -0.00015,
    -0.129,     -0.2541,    -0.38,      -0.5061259, -0.63235,   -0.7597898,
    -0.8872418, -1.012168,  -1.141,     -1.2662,    -1.393,     -1.519337,
    -1.646275,  -1.773301,  -1.900587,  -2.026912,  -2.155,     -2.2815,
    -2.409,     -2.534932,  -2.662501,  -2.789024,  -2.916055,  -3.043698,
    -3.172,     -3.299,     -3.425,     -3.552222,  -3.680335,  -3.806265,
    -3.932954,  -4.06183,   -4.19,      -4.318,     -4.444,     -4.570508,
    -4.699079,  -4.824327,  -4.950584,  -5.080608,  -5.209,     -5.336,
    -5.463,     -5.589088,  -5.718031,  -5.842508,  -5.968246,  -6.099661,
    -6.607262,  -7.117295,  -7.624327,  -8.133802,  -8.639587,  -9.149,
    -9.652353,  -10.16,     -10.665443, -11.17,     -11.671568, -12.174,
    -12.673194, -13.173,    -13.669682, -14.166,    -14.660411, -15.154,
    -15.644783, -16.135,    -16.622221, -17.106088, -17.592171, -18.071976,
    -18.54765,  -19.029597, -19.50071,  -19.978461, -20.44479,  -20.918108,
    -21.37943,  -21.848107, -22.30422,  -22.768055, -23.21878,  -23.677577,
    -24.12274,  -25.01577};

static const float hs128_azimuth_offset_map[] = {
    3.257,         3.263,         1.091414,      3.268,     1.092504,
    3.273,         1.093621,      3.278,         1.094766,  3.283,
    1.095941,      3.288,         1.097146,      3.291,     1.098384,
    -1.101114,     1.099655,      -1.103649,     -3.306,    -1.106118,
    -3.311,        -1.108519,     -3.318,        -1.110852, -3.324,
    -1.113115,     7.72,          5.535,         3.325,     -3.33,
    1.10673,       -5.5380349333, -7.7261072476, -1.115309, 7.731,
    5.543,         3.329,         -3.336,        1.108227,  -5.5468131333,
    -7.7382149333, -1.117431,     7.743,         5.550783,  3.335,
    -3.341759,     1.109762,      -5.555361,     -7.750039, -1.119482,
    7.757,         5.559905,      3.34,          -3.347338, 1.111338,
    -5.564415,     -7.762486,     -1.12146,      7.768,     5.568891,
    3.345,         -3.352812,     1.112953,      -5.573332, -7.774765,
    -1.123366,     7.78,          5.577739,      3.351,     -3.358181,
    1.114609,      -5.582111,     -7.786874,     -1.125199, 7.792,
    5.586449,      3.356,         -3.363443,     1.116305,  -5.590751,
    -7.798811,     -1.126958,     7.804,         5.595019,  3.36,
    -3.368599,     1.118042,      -5.599251,     -7.810576, -1.128643,
    -3.373647,     -1.130255,     -3.378587,     -1.131792, -3.383419,
    3.381,         -3.388143,     3.386,         1.127077,  3.39,
    1.129045,      3.395,         1.131048,      3.401,     1.133088,
    3.406,         1.135163,      3.41,          1.137272,  3.416,
    1.139412,      -1.142319,     1.141584,      -1.143128, -3.425708,
    -1.143867,     -3.429329,     -1.144538,     -3.432841, -1.14514,
    -3.436242,     -1.145675,     -3.439533,     -1.146145, -3.442714,
    -1.146549,     -3.445786,     -3.448749};

Pandar128Driver::Pandar128Driver() {
  constexpr int kLaserNum = 128;
  elev_angle_map_.resize(kLaserNum);
  horizatal_azimuth_offset_map_.resize(kLaserNum);
  distance_offset_map_.resize(kLaserNum);
  for (int i = 0; i < kLaserNum; ++i) {
    elev_angle_map_[i] = hs128_elev_angle_map[i];
    horizatal_azimuth_offset_map_[i] = hs128_azimuth_offset_map[i];
    distance_offset_map_[i] = 0;
  }
  //
  for (uint16_t rotIndex = 0; rotIndex < cos_lookup_table_.size(); ++rotIndex) {
    const double rotation = kDegreeToRadian * (static_cast<double>(rotIndex) /
                                               kSinCosTableResolution);
    cos_lookup_table_[rotIndex] = cos(rotation);
    sin_lookup_table_[rotIndex] = sin(rotation);
  }
  min_laser_azimuth_offset_ = 360.0;
  max_laser_azimuth_offset_ = -360.0;
  // init point cloud buffer
  for (size_t i = 0; i < max_point_cloud_buffer_size_; i++) {
    auto point_cloud = std::make_shared<PointCloud>();
    point_cloud->points.reserve(720000);
    point_cloud->udp_scan_sequence.reserve(4000);
    point_cloud_buffer_.push_back(point_cloud);
  }
  resetDriver();
}

Pandar128Driver::~Pandar128Driver() {
  // Stop threads in case ParseLidarPacket() called
  // after HesaiTestDriver destructed
  // Stop();
}

void Pandar128Driver::resetDriver() {
  cur_packet_index_ = 0;
  point_cloud_ = point_cloud_buffer_[point_cloud_buffer_index_];
  point_cloud_->points.clear();
  point_cloud_->udp_scan_sequence.clear();
  point_cloud_->udp_packet_number = 0;
  point_cloud_buffer_index_ =
      (point_cloud_buffer_index_ + 1) % point_cloud_buffer_.size();
  accumulate_azimuth_ = 0;
}


void Pandar128Driver::resetFrameIndex() {
  index_f_single_thread_ = 0;
}

bool Pandar128Driver::ParseLidarPacket(const uint8_t* data, size_t len) {
  if (len != kHsLidarL128_PacketSize && len != kHsLidarL128_CSC_PacketSize) {
    return false;
  }

  auto hs128_pkt = reinterpret_cast<const Pandar128Packet*>(data);
  if (hs128_pkt->header.sop != 0xFFEE) {
    std::cerr << "Start of Packet error: " << std::hex << hs128_pkt->header.sop;
    return false;
  }
  if (hs128_pkt->header.protocalVersionMajor != 0x01 ||
      hs128_pkt->header.protocalVersionMinor != 0x04) {
    std::cerr << "Lidar UDP packet format version: "
              << hs128_pkt->header.protocalVersionMajor << '.'
              << hs128_pkt->header.protocalVersionMinor << ". New firmware?";
    return false;
  }
  return_mode_ = hs128_pkt->return_mode;
  is_dual_return_ = hs128_pkt->return_mode == 0x39 ||
                    hs128_pkt->return_mode == 0x3B ||
                    hs128_pkt->return_mode == 0x3C;

  struct tm tTm;
  tTm.tm_year = hs128_pkt->addtime[0];
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = hs128_pkt->addtime[1] - 1;
  tTm.tm_mday = hs128_pkt->addtime[2];
  tTm.tm_hour = hs128_pkt->addtime[3];
  tTm.tm_min = hs128_pkt->addtime[4];
  tTm.tm_sec = hs128_pkt->addtime[5];
  tTm.tm_isdst = 0;
  double pkt_time_ns = static_cast<double>(timegm(&tTm)) * 1e9 +
                       (static_cast<uint64_t>(hs128_pkt->timestamp)) * 1000;

  // point_cloud_->set_height(kHsLidarL128UnitNum);
  if(point_cloud_->points.empty()){
    point_cloud_->begin_index_in_pacp = cur_packet_index_;
  }
  point_cloud_->udp_packet_number++;
  for (int i = 0; i < hs128_pkt->header.chBlockNumber; ++i) {
    if (is_dual_return_ && i % 2 == 1) {
      // The second return will be parsed together with the first return,
      // In the dual return mode, We assume that the block number is always even
      // as we have observed in all the Hesai products so far.
      continue;
    }
    const int cur_azimuth = hs128_pkt->blocks[i].azimuth;
    // std::cout<<"cur_azimuth: " << cur_azimuth << ", accumulate_azimuth_: " <<
    // accumulate_azimuth_ <<std::endl;
    if (CheckAngle(cur_azimuth) && point_cloud_->points.size() > 0 &&
        point_cloud_callback_) {
      point_cloud_->end_index_in_pacp = cur_packet_index_;
      CheckLidarHealth(hs128_pkt);
      point_cloud_->timestamp = point_cloud_->points[0].timestamp / 1e9;
      point_cloud_->return_mode = return_mode_;
      point_cloud_callback_(point_cloud_, index_f_single_thread_++);
      // clear
      resetDriver();
    }
    CalcBlockXYZIT(hs128_pkt, i, pkt_time_ns);
    last_azimuth_ = cur_azimuth;
  }
  // point_cloud_->set_is_motor_clockwise(azimuth_sign_ == 1);
  return true;
}

void Pandar128Driver::CalcBlockXYZIT(const Pandar128Packet* pkt,
                                     const int blockid, double pkt_time_ns) {
  static LasersTSOffset laser_ts_offset(GLOBAL_CONFIG_DIR
                                        "/Pandar128_laser_ts.csv");
  const Pandar128Block* block = &pkt->blocks[blockid];

  const int mode = pkt->operational_state;
  const int state = blockid == 0 ? ((pkt->azimuth_flag) >> 14)
                                 : ((pkt->azimuth_flag) >> 12) & 0x0003;

  constexpr int kOffset1 = 3148;
  constexpr int kOffset2 = 27778;
  pkt_time_ns += kOffset1;
  if (is_dual_return_ == false) {
    if (blockid % 2 == 0) {
      pkt_time_ns -= kOffset2 * (mode == 0 ? 1 : 2);
    }
  }
  // If current azimuth is out of lidar range(FOV), Do not calculate.
  if (!CheckLidarRange(pkt->blocks[blockid].azimuth)) {
    std::cout << "Current azimuth is out of lidar range.";
    return;
  }
  uint32_t scans_per_udp = 0;
  uint64_t scan_sequence = 0;
  if (is_dual_return_) {
    scans_per_udp = pkt->header.chBlockNumber >> 1;
    scan_sequence =
        static_cast<uint64_t>(pkt->udpSequence) * scans_per_udp + blockid / 2;
  } else {
    scans_per_udp = pkt->header.chBlockNumber;
    scan_sequence =
        static_cast<uint64_t>(pkt->udpSequence) * scans_per_udp + blockid;
  }
  // point_cloud_->set_scans_per_udp(scans_per_udp);
  // point_cloud_->add_scan_sequence(scan_sequence);
  // point_cloud_->add_scan_timestamp(pkt_time_ns);
  // point_cloud_->add_scan_azimuth(static_cast<float>(block->azimuth) / 100.0);
  point_cloud_->udp_scan_sequence.push_back(scan_sequence);
  std::vector<float> distance;
  std::vector<uint8_t> intensity;
  distance.reserve(2);
  intensity.reserve(2);
  const float dis_unit = pkt->header.chDisUnit / 1000.;
  float encoder = static_cast<float>(block->azimuth) / 100.0;
  for (int i = 0; i < pkt->header.chLaserNumber; ++i) {
    /* for all the units in a block */
    const Pandar128Unit& unit = block->units[i];
    distance.clear();
    intensity.clear();
    distance.push_back(unit.distance * dis_unit);
    intensity.push_back(unit.reflectivity);
    if (is_dual_return_) {
      const auto& second_unit = pkt->blocks[blockid + 1].units[i];
      if (unit.distance != second_unit.distance) {
        distance.push_back(second_unit.distance * dis_unit);
        intensity.push_back(second_unit.reflectivity);
      }
    }
    float laser_pitch = elev_angle_map_[i];
    float laser_azimuth = horizatal_azimuth_offset_map_[i] +
                          (static_cast<float>(block->azimuth)) / 100.0;
    for (size_t j = 0; j < distance.size(); j++) {
      /* skip wrong points */
      const double dist_in_m = distance[j] + distance_offset_map_[i];
      if (dist_in_m <= min_distance_ || dist_in_m > max_distance_) {
        continue;
      }
      int offset = azimuth_sign_ *
                   laser_ts_offset.getTSOffset(i, mode, state, dist_in_m);
      float azimuth = laser_azimuth +
                      laser_ts_offset.getAngleOffset(offset, pkt->motor_speed);
      float pitch =
          laser_pitch + laser_ts_offset.getPitchOffset(laser_pitch, dist_in_m);
      float xy_dist = dist_in_m * CosLookupDegree(pitch);
      azimuth += laser_ts_offset.getAzimuthOffset(
          horizatal_azimuth_offset_map_[i], xy_dist);
      if (azimuth > 360) {
        azimuth -= 360;
      }
      if (azimuth < 0) {
        azimuth += 360;
      }
      Point p;
      p.timestamp = pkt_time_ns + offset;
      p.laser_id = i;
      p.return_id = (distance.size() == 1) ? 0 : j + 1;
      p.encoder = encoder;
      p.azimuth = azimuth;
      p.elevation = pitch;
      p.y = xy_dist * SinLookupDegree(azimuth);
      p.x = -xy_dist * CosLookupDegree(azimuth);
      p.z = dist_in_m * SinLookupDegree(pitch);
      p.distance = dist_in_m;
      p.intensity = intensity[j];
      point_cloud_->points.push_back(p);
    }
  }
}

bool Pandar128Driver::CheckLidarHealth(const Pandar128Packet* pkt) {
  if (pkt->functional_safety.FSVersion != 0x00) {
    return false;
  }

  if (pkt->functional_safety.lidarStateInfo.IsCurrentFault() == false) {
    fault_level_ = FaultLevel::NF;
    health_report_.clear();
    return true;
  }

  const auto& fc = pkt->functional_safety.faultCode;
  switch (pkt->functional_safety.lidarStateInfo.LidarState()) {
    case LidarStateInfo::State::WARN:
      if (fc != 0x0681) {  // 0x0681 is noisy while non critical
        fault_level_ = FaultLevel::FL1;
        health_report_ = "LidarState: WARN; ";
        health_report_.append(
            GetFaultCodeDescription(fc).value_or("Unknown fault code"));
      }
      break;
    case LidarStateInfo::State::PRE_PERF_DEGRADATION:
      fault_level_ = FaultLevel::FL2;
      health_report_ = "LidarState: Pre-performance degradation; ";
      health_report_.append(
          GetFaultCodeDescription(fc).value_or("Unknown fault code"));
      break;
    case LidarStateInfo::State::PERF_DEGRADATION:
      fault_level_ = FaultLevel::FL2;
      health_report_ = "LidarState: Performance degradation; ";
      health_report_.append(
          GetFaultCodeDescription(fc).value_or("Unknown fault code"));
      break;
    case LidarStateInfo::State::PRE_SHUTDOWN:
      fault_level_ = FaultLevel::FL3;
      health_report_ = "LidarState: Pre-shutdown; ";
      health_report_.append(
          GetFaultCodeDescription(fc).value_or("Unknown fault code"));
      break;
    case LidarStateInfo::State::SHUTDOWN:
      fault_level_ = FaultLevel::FL3;
      health_report_ = "LidarState: Shutdown; ";
      health_report_.append(
          GetFaultCodeDescription(fc).value_or("Unknown fault code"));
      break;
    case LidarStateInfo::State::STANDBY_MODE:
      fault_level_ = FaultLevel::NF;
      health_report_ = "LidarState: Standby; ";
      break;
    case LidarStateInfo::State::INIT:
    case LidarStateInfo::State::NORMAL:
      fault_level_ = FaultLevel::NF;
      health_report_.clear();
      break;
  }
  return true;
}

bool Pandar128Driver::CheckLidarRange(const int cur_azimuth) const {
  if (lidar_ranges_.empty()) {
    return true;  // If have not got lidar ranges, Do not influence the logic.
  }
  for (const std::pair<int, int>& range : lidar_ranges_) {
    const int start = range.first * 10;  // Lidar start angle range [0, 36000]
    const int end = range.second * 10;   // Lidar end angle range [0, 36000]
    const int min_laser_azimuth =
        (cur_azimuth +
         static_cast<int>(std::floor(min_laser_azimuth_offset_ * 100)) - 10 +
         36000) %
        36000;
    const int max_laser_azimuth =
        (cur_azimuth +
         static_cast<int>(std::ceil(max_laser_azimuth_offset_ * 100)) + 10) %
        36000;
    if (start > end) {  // e.g. [27000, 9000] => [27000, 36000] U [0, 9000]
      if (max_laser_azimuth >= start || min_laser_azimuth <= end) {
        return true;  // At least one of laser azimuth is in fov range.
      }
    } else if ((max_laser_azimuth >= start && max_laser_azimuth <= end) ||
               (min_laser_azimuth >= start && min_laser_azimuth <= end)) {
      return true;  // At least one of laser azimuth is in fov range.
    }
  }
  return false;
}

float Pandar128Driver::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return cos_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

float Pandar128Driver::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return sin_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

bool Pandar128Driver::CheckAngle(const int cur_azimuth) {
  int azimuth_gap = 0;
  constexpr int kAzimuthThre = 200;
  if (cur_azimuth - last_azimuth_ > 0 &&
      cur_azimuth - last_azimuth_ < kAzimuthThre) {
    azimuth_sign_ = 1;  // clockwise
  } else if (last_azimuth_ - cur_azimuth > 0 &&
             last_azimuth_ - cur_azimuth < kAzimuthThre) {
    azimuth_sign_ = -1;  // countclockwise
  }
  if (azimuth_sign_ * (last_azimuth_ - cur_azimuth) > 0) {
    azimuth_gap = 36000 + azimuth_sign_ * (static_cast<int>(cur_azimuth) -
                                           static_cast<int>(last_azimuth_));
  } else {
    azimuth_gap = azimuth_sign_ * (static_cast<int>(cur_azimuth) -
                                   static_cast<int>(last_azimuth_));
  }
  accumulate_azimuth_ += azimuth_gap;
  if (last_azimuth_ != cur_azimuth) {
    if ((azimuth_sign_ * (last_azimuth_ - cur_azimuth) > 0 &&
         (azimuth_sign_ * (start_angle_ - cur_azimuth) <= 0 ||
          azimuth_sign_ * (start_angle_ - last_azimuth_) > 0)) ||
        (azimuth_sign_ * (last_azimuth_ - start_angle_) < 0 &&
         azimuth_sign_ * (start_angle_ - cur_azimuth) <= 0)) {
      constexpr int kMinPublishFOV = 30000;  // 300degree
      return accumulate_azimuth_ > kMinPublishFOV;
    }
  }
  return false;
}

bool Pandar128Driver::setIntrinsics(const std::vector<double>& elevation,
                                    const std::vector<double>& azimuth,
                                    const std::vector<double>& distance) {
  elev_angle_map_ = elevation;
  horizatal_azimuth_offset_map_ = azimuth;
  distance_offset_map_ = distance;
  return true;
}

bool Pandar128Driver::getIntrinsics(std::vector<double>& elevation,
                                    std::vector<double>& azimuth,
                                    std::vector<double>& distance) {
  elevation = elev_angle_map_;
  azimuth = horizatal_azimuth_offset_map_;
  distance = distance_offset_map_;
  return true;
}

}  // namespace hesai
}  // namespace drivers
}  // namespace autox
