/******************************************************************************
 * Copyright 2021 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#include "driver/driver_qt64.h"

#include <sys/types.h>

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

namespace autox {
namespace drivers {
namespace hesai_qt64 {

std::array<double, kRotationMaxUnits> DriverQt64::sin_lookup_table_;
std::array<double, kRotationMaxUnits> DriverQt64::cos_lookup_table_;

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding

typedef struct PandarQtHeader_s {
  char sop[2];                // 0xEEFF 2bytes
  char protocalVersionMajor;  // 0x03
  char protocalVersionMinor;  // 0x01
  char reserved[2];

  unsigned char chLaserNumber;  // laser number: 0x40 or 64
  unsigned char chBlockNumber;  // block number: 0x04
  char echoCount;               // 0x01: The first block is the 1st return.
                                // 0x03: The first block is the 2nd return
  char chDisUnit;               // Distance unit, 6mm/5mm/4mm
  char echoNum;                 // 0x01: single return ; 0x02: dual return
  char hasUdpSequence;          // 0x01: has UdpSequence in UDP packet
} PandarQtHeader;

typedef struct PandarQtUnit_s {
  uint16_t distance;           // *4mm, max distance: (2^16-1) * 4mm = 262.14m
  unsigned char reflectivity;  // reflectivity: 0 to 255%
  unsigned char backgroudIllumination;
} PandarQtUnit;

constexpr unsigned int kHsLidarQtUnitNum = 64;

typedef struct PandarQtBlock_s {
  uint16_t azimuth;  //  [azimuth[1],azimuth[0]] / 100 degree
  PandarQtUnit units[kHsLidarQtUnitNum];
} PandarQtBlock;

constexpr unsigned int kHsLidarQtBlockNumber_4 = 4;

struct PandarQtPacket_s {
  PandarQtHeader header;
  PandarQtBlock blocks[kHsLidarQtBlockNumber_4];

  // packet tail
  char reserved[6];
  unsigned char highTempShutDownFlag;  // 0x01: pause due to overheat
  char reserved2[3];
  uint16_t motorSpeed;       // RPM
  uint32_t timestamp;        // 0 to 1000,000 us (1s)
  unsigned char returnMode;  // 0x33: First; 0x38: Last; 0x3B: dual
  char factoryInfo;          // 0x42 or 0x43
  unsigned char year;
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;
  unsigned char second;

  uint32_t udpSequence;
};

#pragma pack(pop)
constexpr unsigned int kHsLidarQtPacketSize = sizeof(PandarQtPacket);  // 1072

static const float elev_angle_map[] = {
    -52.404, -50.087, -47.903, -45.806, -43.771, -41.833, -39.933, -38.096,
    -36.314, -34.575, -32.867, -31.192, -29.538, -27.908, -26.312, -24.731,
    -23.188, -21.627, -20.086, -18.565, -17.048, -15.555, -14.063, -12.576,
    -11.091, -9.62,   -8.139,  -6.673,  -5.217,  -3.76,   -2.306,  -0.842,
    0.605,   2.065,   3.521,   4.974,   6.44,    7.91,    9.372,   10.84,
    12.328,  13.816,  15.297,  16.804,  18.315,  19.84,   21.373,  22.914,
    24.497,  26.077,  27.676,  29.301,  30.945,  32.617,  34.328,  36.069,
    37.848,  39.662,  41.554,  43.471,  45.484,  47.548,  49.731,  52.04};

static const float azimuth_offset_map[] = {
    8.359,  7.975,  7.666,  7.419,  7.191,  6.986,  6.821,  6.664,
    6.534,  6.411,  6.311,  6.222,  6.133,  6.058,  5.987,  5.924,
    -5.393, -5.329, -5.268, -5.217, -5.17,  -5.125, -5.092, -5.063,
    -5.027, -5.002, -4.977, -4.953, -4.946, -4.94,  -4.936, -4.934,
    5.502,  5.5,    5.507,  5.511,  5.526,  5.537,  5.55,   5.57,
    5.594,  5.621,  5.647,  5.689,  5.724,  5.763,  5.81,   5.858,
    -5.454, -5.526, -5.61,  -5.701, -5.808, -5.925, -6.047, -6.197,
    -6.361, -6.534, -6.73,  -6.969, -7.233, -7.548, -7.907, -8.342};

DriverQt64::DriverQt64() {
  constexpr int kLaserNum = 64;
  elev_angle_map_.resize(kLaserNum);
  horizatal_azimuth_offset_map_.resize(kLaserNum);
  for (int i = 0; i < kLaserNum; ++i) {
    elev_angle_map_[i] = elev_angle_map[i];
    horizatal_azimuth_offset_map_[i] = azimuth_offset_map[i];
  }
  block_64_offset_[0] = 25.71f;
  block_64_offset_[1] = 25.71f + 166.67f;
  block_64_offset_[2] = 25.71f + 333.33f;
  block_64_offset_[3] = 25.71f + 500.00f;

  block_64_offset_dual_[0] = 25.71f;
  block_64_offset_dual_[1] = 25.71f;
  block_64_offset_dual_[2] = 25.71f + 166.67f;
  block_64_offset_dual_[3] = 25.71f + 166.67f;

  laser_64_offset_[0] = 10.0f + 2.31f;
  laser_64_offset_[1] = 10.0f + 4.37f;
  laser_64_offset_[2] = 10.0f + 6.43f;
  laser_64_offset_[3] = 10.0f + 8.49f;
  laser_64_offset_[4] = 10.0f + 10.54f;
  laser_64_offset_[5] = 10.0f + 12.60f;
  laser_64_offset_[6] = 10.0f + 14.66f;
  laser_64_offset_[7] = 10.0f + 16.71f;
  laser_64_offset_[8] = 10.0f + 19.16f;
  laser_64_offset_[9] = 10.0f + 21.22f;
  laser_64_offset_[10] = 10.0f + 23.28f;
  laser_64_offset_[11] = 10.0f + 25.34f;
  laser_64_offset_[12] = 10.0f + 27.39f;
  laser_64_offset_[13] = 10.0f + 29.45f;
  laser_64_offset_[14] = 10.0f + 31.50f;
  laser_64_offset_[15] = 10.0f + 33.56f;

  laser_64_offset_[16] = 10.0f + 36.61f;
  laser_64_offset_[17] = 10.0f + 38.67f;
  laser_64_offset_[18] = 10.0f + 40.73f;
  laser_64_offset_[19] = 10.0f + 42.78f;
  laser_64_offset_[20] = 10.0f + 44.84f;
  laser_64_offset_[21] = 10.0f + 46.90f;
  laser_64_offset_[22] = 10.0f + 48.95f;
  laser_64_offset_[23] = 10.0f + 51.01f;
  laser_64_offset_[24] = 10.0f + 53.45f;
  laser_64_offset_[25] = 10.0f + 55.52f;
  laser_64_offset_[26] = 10.0f + 57.58f;
  laser_64_offset_[27] = 10.0f + 59.63f;
  laser_64_offset_[28] = 10.0f + 61.69f;
  laser_64_offset_[29] = 10.0f + 63.74f;
  laser_64_offset_[30] = 10.0f + 65.80f;
  laser_64_offset_[31] = 10.0f + 67.86f;

  laser_64_offset_[32] = 10.0f + 70.90f;
  laser_64_offset_[33] = 10.0f + 72.97f;
  laser_64_offset_[34] = 10.0f + 75.02f;
  laser_64_offset_[35] = 10.0f + 77.08f;
  laser_64_offset_[36] = 10.0f + 79.14f;
  laser_64_offset_[37] = 10.0f + 81.19f;
  laser_64_offset_[38] = 10.0f + 83.25f;
  laser_64_offset_[39] = 10.0f + 85.30f;
  laser_64_offset_[40] = 10.0f + 87.75f;
  laser_64_offset_[41] = 10.0f + 89.82f;
  laser_64_offset_[42] = 10.0f + 91.87f;
  laser_64_offset_[43] = 10.0f + 93.93f;
  laser_64_offset_[44] = 10.0f + 95.98f;
  laser_64_offset_[45] = 10.0f + 98.04f;
  laser_64_offset_[46] = 10.0f + 100.10f;
  laser_64_offset_[47] = 10.0f + 102.15f;

  laser_64_offset_[48] = 10.0f + 105.20f;
  laser_64_offset_[49] = 10.0f + 107.26f;
  laser_64_offset_[50] = 10.0f + 109.32f;
  laser_64_offset_[51] = 10.0f + 111.38f;
  laser_64_offset_[52] = 10.0f + 113.43f;
  laser_64_offset_[53] = 10.0f + 115.49f;
  laser_64_offset_[54] = 10.0f + 117.54f;
  laser_64_offset_[55] = 10.0f + 119.60f;
  laser_64_offset_[56] = 10.0f + 122.05f;
  laser_64_offset_[57] = 10.0f + 124.11f;
  laser_64_offset_[58] = 10.0f + 126.17f;
  laser_64_offset_[59] = 10.0f + 128.22f;
  laser_64_offset_[60] = 10.0f + 130.28f;
  laser_64_offset_[61] = 10.0f + 132.34f;
  laser_64_offset_[62] = 10.0f + 134.39f;
  laser_64_offset_[63] = 10.0f + 136.45f;
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

DriverQt64::~DriverQt64() {
  // Stop threads in case ParseLidarPacket() called
  // after DriverQt64 destructed
  // Stop();
}

bool DriverQt64::ParseLidarPacket(const uint8_t* data, size_t len) {
  if (!data || len != kHsLidarQtPacketSize) {
    return false;
  }

  auto qt_pkt = reinterpret_cast<const PandarQtPacket*>(data);
  if (qt_pkt->header.sop[0] != '\xEE' || qt_pkt->header.sop[1] != '\xFF') {
    std::cout << "Start of Packet error: " << std::hex
              << (0xFF & qt_pkt->header.sop[0])
              << (0xFF & qt_pkt->header.sop[1]);
    return false;
  }
  if (qt_pkt->header.protocalVersionMajor != 0x03 ||
      qt_pkt->header.protocalVersionMinor != 0x01) {
    std::cout << "Lidar UDP packet format version: "
              << qt_pkt->header.protocalVersionMajor << '.'
              << qt_pkt->header.protocalVersionMinor << ". New firmware?";
    return false;
  }

  is_dual_return_ = (qt_pkt->header.echoNum == '\x02');  // '\x01' single return
  struct tm tTm;
  // year since 1900
  tTm.tm_year = qt_pkt->year;
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = qt_pkt->month - 1;
  tTm.tm_mday = qt_pkt->day;
  tTm.tm_hour = qt_pkt->hour;
  tTm.tm_min = qt_pkt->minute;
  tTm.tm_sec = qt_pkt->second;
  tTm.tm_isdst = 0;
  auto pkt_timestamp = static_cast<double>(timegm(&tTm));
  // TODO(kewei) consider endianness, e.g. HsByteArrToUint(qt_pkt->timestamp)
  // The sub second part of the timestamp
  const unsigned int timestamp_in_us = qt_pkt->timestamp;
  pkt_timestamp += timestamp_in_us * 1e-6;

  const double distance_unit = qt_pkt->header.chDisUnit / 1000.0;
  if(point_cloud_->points.empty()){
    point_cloud_->begin_index_in_pacp = cur_packet_index_;
  }
  point_cloud_->udp_packet_number++;
  for (int i = 0; i < qt_pkt->header.chBlockNumber; ++i) {  // for each block
    if (is_dual_return_ && i % 2 == 1) {
      // The second return will be parsed together with the first return,
      // In the dual return mode, We assume that the block number is always even
      // as we have observed in all the Hesai products so far.
      continue;
    }
    const PandarQtBlock& block = qt_pkt->blocks[i];
    const int azimuth = block.azimuth;
    if (CheckAngle(azimuth) && point_cloud_->points.size() > 0 &&
        point_cloud_callback_) {
      point_cloud_->end_index_in_pacp = cur_packet_index_;
      point_cloud_->timestamp = point_cloud_->points[0].timestamp / 1e9;
      point_cloud_->return_mode = qt_pkt->returnMode;
      point_cloud_callback_(point_cloud_, index_f_single_thread_);
      // clear
      resetDriver();
    }
    CalcPointXYZIT(qt_pkt, i, qt_pkt->header.chLaserNumber,
                   static_cast<uint64_t>(pkt_timestamp * 1e9), distance_unit,
                   azimuth / 100.0);
    last_azimuth_ = azimuth;
  }
  return true;
}

void DriverQt64::CalcPointXYZIT(const PandarQtPacket* qt_pkt,
                                const unsigned int blockid,
                                const unsigned int laser_num,
                                const uint64_t pkt_timestamp,
                                const double distance_unit,
                                const double azimuth) {
  // for all the units/points in a block
  uint32_t scans_per_udp = 0;
  uint64_t scan_sequence = 0;
  if (is_dual_return_) {
    scans_per_udp = qt_pkt->header.chBlockNumber >> 1;
    scan_sequence = static_cast<uint64_t>(qt_pkt->udpSequence) * scans_per_udp +
                    blockid / 2;
  } else {
    scans_per_udp = qt_pkt->header.chBlockNumber;
    scan_sequence =
        static_cast<uint64_t>(qt_pkt->udpSequence) * scans_per_udp + blockid;
  }
  point_cloud_->udp_scan_sequence.push_back(scan_sequence);
  std::vector<float> distance;
  std::vector<uint8_t> intensity;
  distance.reserve(2);
  intensity.reserve(2);
  const auto& block = qt_pkt->blocks[blockid];
  for (unsigned int i = 0; i < laser_num; ++i) {
    const auto& unit = block.units[i];
    distance.clear();
    intensity.clear();
    distance.push_back(unit.distance * distance_unit);
    intensity.push_back(unit.reflectivity & 0xff);
    if (is_dual_return_) {
      const auto& second_unit = qt_pkt->blocks[blockid + 1].units[i];
      if (unit.distance != second_unit.distance) {
        distance.push_back(second_unit.distance * distance_unit);
        intensity.push_back(second_unit.reflectivity & 0xff);
      }
    }
    for (size_t j = 0; j < distance.size(); j++) {
      const double dist_in_m = distance[j];
      if (dist_in_m <= min_distance_ || dist_in_m > max_distance_) {
        continue;
      }
      double xy_dist = dist_in_m * CosLookupDegree(elev_angle_map_[i]);
      float corrected_azimuth = horizatal_azimuth_offset_map_[i] + azimuth - 90;
      float corrected_elevation = elev_angle_map_[i];
      if (corrected_azimuth > 360) {
        corrected_azimuth -= 360;
      }
      if (corrected_azimuth < 0) {
        corrected_azimuth += 360;
      }
      Point p;
      uint64_t offset = (static_cast<uint64_t>(block_64_offset_[blockid] +
                                               laser_64_offset_[i]) *
                         1000);
      p.timestamp = pkt_timestamp - offset;
      p.laser_id = i;
      p.return_id = (distance.size() == 1) ? 0 : j + 1;
      p.encoder = azimuth;
      p.azimuth = corrected_azimuth;
      p.elevation = corrected_elevation;
      p.x = xy_dist * SinLookupDegree(corrected_azimuth);
      p.y = xy_dist * CosLookupDegree(corrected_azimuth);
      p.z = dist_in_m * SinLookupDegree(corrected_elevation);
      p.distance = dist_in_m;
      p.intensity = intensity[j];
      point_cloud_->points.push_back(p);
    }
  }
}

float DriverQt64::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return cos_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

float DriverQt64::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return sin_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

bool DriverQt64::CheckAngle(const int cur_azimuth) {
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

void DriverQt64::resetDriver() {
  point_cloud_ = point_cloud_buffer_[point_cloud_buffer_index_];
  point_cloud_->points.clear();
  point_cloud_->udp_scan_sequence.clear();
  point_cloud_->udp_packet_number = 0;
  point_cloud_buffer_index_ =
      (point_cloud_buffer_index_ + 1) % point_cloud_buffer_.size();
  accumulate_azimuth_ = 0;
}

void DriverQt64::resetFrameIndex() {
  index_f_single_thread_ = 0;
}

}  // namespace hesai_qt64
}  // namespace drivers
}  // namespace autox
