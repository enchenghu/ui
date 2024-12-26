/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "driver/blidar_driver.h"

#include <sys/types.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "utils/utils.h"

namespace autox {
namespace drivers {
namespace blidar {

std::array<double, kRotationMaxUnits> BLidarDriver::sin_lookup_table_;
std::array<double, kRotationMaxUnits> BLidarDriver::cos_lookup_table_;

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding

typedef struct LidarHeader_s {
  char sop[2];                // 0xEEFF 2bytes
  char protocalVersionMajor;  // 0x03
  char protocalVersionMinor;  // 0x01
  char reserved[2];

  unsigned char chLaserNumber;  // laser number: 0x04
  unsigned char chBlockNumber;  // block number: 0x3c (60)
  char returnId;        // 0x00: the first return, 0x01: the second return.
  char chDisUnit;       // Distance unit, 6mm/5mm/4mm
  char returnNum;       // 0x01: single return ; 0x02: dual return
  char hasUdpSequence;  // 0x01: has UdpSequence in UDP packet
} LidarHeader;

typedef struct LidarUnit_s {
  uint16_t distance;           // *4mm, max distance: (2^16-1) * 4mm = 262.14m
  unsigned char reflectivity;  // reflectivity: 0 to 255%
  unsigned char backgroudIllumination;
} LidarUnit;

typedef struct LidarBlock_s {
  uint16_t azimuth;    //  [azimuth[1],azimuth[0]] / 100 degree
  uint16_t elevation;  //  [elevation[1],elevation[0]] / 100 degree
  LidarUnit units[kLidarUnitNum];
} LidarBlock;

typedef struct PacketTail_s {
  // packet tail
  char reserved[6];
  unsigned char highTempShutDownFlag;  // 0x01: pause due to overheat
  unsigned char isLastPacket;
  char reserved2[2];
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
} PacketTail;

struct LidarPacket_s {
  LidarHeader header;
  LidarBlock blocks[kLidarBlockNum];
  PacketTail tail;
};

// for protocol 5.2
typedef struct {
  uint8_t sop[2];         // 0xEEFF 2bytes
  uint8_t major_version;  // 0x04
  uint8_t minor_version;  // 0x02
  uint16_t data_size;
  uint8_t reserved[2];
} PacketHeader;

typedef struct {
  uint16_t distance;     // *1mm, max distance: (2^16-1) * 1mm = 65.535m
  uint8_t reflectivity;  // reflectivity: 0 to 255%
  uint8_t elongation;
} ReturnUnit5_2;

typedef struct {
  uint8_t return_num;
  ReturnUnit5_2 returns[0];
} ColumnUnit5_2;

typedef struct {
  PacketHeader header;
  // data head
  uint16_t encoder;
  uint8_t frame_id;
  uint8_t laser_id;
  uint16_t unit_start_id;
  uint16_t unit_column_num;
  uint8_t max_return_num;
  uint8_t distance_unit;
  uint8_t column_id;
  uint8_t reserved;
  // data
  uint8_t laser_data[0];
} LidarPacket5_2;

#pragma pack(pop)
constexpr unsigned int kLidarPacketSize = sizeof(LidarPacket);  // 1284

static const float g_elevation_offset_map[] = {-60, -60, -60, -60};

static const float g_azimuth_offset_map[] = {-3, -1, 1, 3};

static const float g_azimuth_offset_map_a2[] = {-0.9, -0.3, 0.3, 0.9};

static const float g_start_frame[] = {78, 198, 318};
static const float g_end_frame[] = {198, 318, 78};

inline double normalize_angle(double a) {
  while (a > 360) {
    a -= 360;
  }
  while (a < 0) {
    a += 360;
  }
  return a;
}

BLidarDriver::BLidarDriver() {
  // init lidar intrinsics
  for (int i = 0; i < kLidarUnitNum; ++i) {
    elevation_offset_map_[i] = g_elevation_offset_map[i];
    azimuth_offset_map_[i] = g_azimuth_offset_map[i];
    distance_offset_map_[i] = 0;
  }
  start_frame_.resize(kMirrorNum);
  end_frame_.resize(kMirrorNum);
  for (int i = 0; i < kMirrorNum; ++i) {
    start_frame_[i] = g_start_frame[i];
    end_frame_[i] = g_end_frame[i];
  }
  // time offset
  block_64_offset_[0] = 25.71f;
  for (int i = 0; i < kLidarBlockNum; ++i) {
    block_64_offset_[i] = 25.71f + 166.67f * i;
    block_64_offset_dual_[i] = 25.71f + 166.67f * int(i / 2);
  }
  laser_64_offset_[0] = 10.0f + 2.31f;
  laser_64_offset_[1] = 10.0f + 4.37f;
  laser_64_offset_[2] = 10.0f + 6.43f;
  laser_64_offset_[3] = 10.0f + 8.49f;
  // init  sin/cos table
  for (uint16_t rotIndex = 0; rotIndex < cos_lookup_table_.size(); ++rotIndex) {
    const double rotation = kDegreeToRadian * (static_cast<double>(rotIndex) /
                                               kSinCosTableResolution);
    cos_lookup_table_[rotIndex] = cos(rotation);
    sin_lookup_table_[rotIndex] = sin(rotation);
  }
  //
  resetDriver();
}

BLidarDriver::~BLidarDriver() {}

void BLidarDriver::setPointCloudCallback(PointCloudCallbackType cb) {
  point_cloud_callback_ = cb;
};

void BLidarDriver::resetDriver() {
  cur_packet_index_ = 0;
  last_packet_count_ = 0;
  point_cloud_ = std::make_shared<PointCloud>();
  point_cloud_->points.clear();
  point_cloud_->udp_sequences.clear();
  point_cloud_->udp_packet_number = 0;
}

void BLidarDriver::resetDriverLoad() { index_f_single_thread_ = 0; }
bool BLidarDriver::ParseLidarPacket(const uint8_t* data, size_t len) {
  // parse udp packet
  if (!ParsePacket(data, len, point_cloud_)) {
    return false;
  }
  point_cloud_->udp_packet_number++;
  // check if a pointcloud frame assembled
  auto packet_tail =
      reinterpret_cast<const PacketTail*>(data + (len - size_t(28)));
  if (packet_tail->isLastPacket && point_cloud_->points.size() > 0 &&
      point_cloud_callback_) {
    last_packet_count_++;
    if (last_packet_count_ > 3) {
      point_cloud_->end_index_in_pacp = cur_packet_index_;
      point_cloud_->offset_in_pcap = cur_packet_offset_;
      // measurement timstamp
      point_cloud_->timestamp = point_cloud_->points[0].timestamp;
      // callback
      point_cloud_callback_(point_cloud_, index_f_single_thread_++);
      // reset
      resetDriver();
    }
  }
  return true;
}

bool BLidarDriver::ParsePacket(const uint8_t* data, size_t len,
                               std::shared_ptr<PointCloud> point_cloud) {
  // check header
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Packet Header Error: " << std::hex << (0xFF & header->sop[0])
       << (0xFF & header->sop[1]) << std::dec;
    LOG(INFO) << ss.str();
    return false;
  }
  // parse udp packet
  if (point_cloud->points.empty()) {
    point_cloud->begin_index_in_pacp = cur_packet_index_;
  }
  if (header->major_version == 0x05 && header->minor_version == 0x01) {
    return ParsePacket5_1(data, len, point_cloud_);
  } else if (header->major_version == 0x05 && header->minor_version == 0x02) {
    return parsePacket5_2(data, len, point_cloud_);
  } else {
    LOG(INFO) << "Invalid packet, packet version: " +
                     std::to_string(header->major_version) + "." +
                     std::to_string(header->minor_version) + ". new firmware?";
  }
  return false;
}

void BLidarDriver::CalcPointXYZIT(const LidarPacket* udp_pkt,
                                  unsigned int block_id, uint8_t return_id,
                                  unsigned int laser_num,
                                  uint64_t pkt_timestamp_ns,
                                  double distance_unit,
                                  std::shared_ptr<PointCloud> point_cloud) {
  const auto& block = udp_pkt->blocks[block_id];
  float azimuth_encoder = block.azimuth * (360.0 / 16320);
  float elevation_encoder = block.elevation * (360.0 / 8000);
  // frame_id
  uint8_t frame_id = 0;
  for (uint8_t i = 0; i < kMirrorNum; i++) {
    if (start_frame_[i] < end_frame_[i]) {
      if (elevation_encoder >= start_frame_[i] &&
          elevation_encoder < end_frame_[i]) {
        frame_id = i;
        break;
      }
    } else {
      if (elevation_encoder > end_frame_[i] ||
          elevation_encoder <= start_frame_[i]) {
        frame_id = i;
        break;
      }
    }
  }
  for (unsigned int i = 0; i < laser_num; ++i) {
    const auto& unit = block.units[i];
    if (unit.distance == 0xffff) {
      continue;
    }
    float dist_in_m = unit.distance * distance_unit;
    if (dist_in_m <= min_distance_ || dist_in_m > max_distance_) {
      continue;
    }
    dist_in_m = dist_in_m + distance_offset_map_[i];
    float corrected_elevation =
        (elevation_encoder - start_frame_[frame_id]) * 2 +
        elevation_offset_map_[i];
    corrected_elevation = normalize_angle(corrected_elevation);
    uint64_t offset = static_cast<uint64_t>(block_64_offset_[block_id] +
                                            laser_64_offset_[i]) *
                      1000;
    // point
    Point p;
    p.timestamp = (pkt_timestamp_ns - offset) / 1e9;
    p.laser_id = i;
    p.return_id = return_id;
    p.azimuth_encoder = block.azimuth;
    p.elevation_encoder = block.elevation;
    p.frame_id = frame_id;
    p.azimuth = azimuth_encoder;  // TODO: fix
    p.elevation = corrected_elevation;
    // for azimuth, y-axis is 0 angle.
    // for elevation, z-axis is 0 angle.
    // clockwise is positive direction
    float lxz = dist_in_m * CosLookupDegree(azimuth_offset_map_[i]);
    float ly = -dist_in_m * SinLookupDegree(azimuth_offset_map_[i]);
    float lx = lxz * SinLookupDegree(corrected_elevation);
    p.x = lx * CosLookupDegree(azimuth_encoder) +
          ly * SinLookupDegree(azimuth_encoder);
    p.y = -lx * SinLookupDegree(azimuth_encoder) +
          ly * CosLookupDegree(azimuth_encoder);
    p.z = lxz * CosLookupDegree(corrected_elevation);
    // mirror x
    p.x = -p.x;
    p.distance = dist_in_m;
    p.intensity = unit.reflectivity;
    point_cloud->points.push_back(p);
  }
}

float BLidarDriver::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  if (angle_in_deg < 0 || angle_in_deg >= 360) {
    LOG(INFO) << "CosLookupDegree error:" + std::to_string(angle_in_deg);
  }
  return cos_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

float BLidarDriver::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  if (angle_in_deg < 0 || angle_in_deg >= 360) {
    LOG(INFO) << "SinLookupDegree error:" + std::to_string(angle_in_deg);
  }
  return sin_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

bool BLidarDriver::setIntrinsics(const std::vector<double>& elevation,
                                 const std::vector<double>& azimuth,
                                 const std::vector<double>& distance) {
  for (int i = 0; i < kLidarUnitNum; i++) {
    elevation_offset_map_[i] = elevation[i];
    azimuth_offset_map_[i] = azimuth[i];
    distance_offset_map_[i] = distance[i];
  }
  return true;
}

bool BLidarDriver::getIntrinsics(std::vector<double>& elevation,
                                 std::vector<double>& azimuth,
                                 std::vector<double>& distance) {
  for (int i = 0; i < kLidarUnitNum; i++) {
    elevation.push_back(elevation_offset_map_[i]);
    azimuth.push_back(azimuth_offset_map_[i]);
    distance.push_back(distance_offset_map_[i]);
  }
  return true;
}

bool BLidarDriver::parsePacket5_2(const uint8_t* data, size_t len,
                                  std::shared_ptr<PointCloud> point_cloud) {
  auto udp_pkt = reinterpret_cast<const LidarPacket5_2*>(data);
  // check len
  if (udp_pkt->header.data_size + (28 + 8) != len) {
    LOG(ERROR) << "Invalid packet, packet size need be data size + 28 + 8, "
                  "packet size:" +
                      std::to_string(len) + ", data size:" +
                      std::to_string(udp_pkt->header.data_size);
    return false;
  }
  if (udp_pkt->header.data_size == 0) {
    return true;
  }
  size_t unit_column_num = udp_pkt->unit_column_num;
  if (udp_pkt->unit_start_id + unit_column_num > 366) {
    LOG(ERROR) << "column out of range. start id: " << udp_pkt->unit_start_id
               << ", unit column number: " << unit_column_num;
    return false;
  }
  float distance_unit = udp_pkt->distance_unit / 1000.0;
  float azimuth = udp_pkt->encoder * (360.0 / 16320);

  size_t data_size = udp_pkt->header.data_size - 12;
  // parse tail
  auto udp_tail =
      reinterpret_cast<const PacketTail*>(data + (len - size_t(28)));
  struct tm tTm;
  // year since 1900
  tTm.tm_year = udp_tail->year;
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = udp_tail->month - 1;
  tTm.tm_mday = udp_tail->day;
  tTm.tm_hour = udp_tail->hour;
  tTm.tm_min = udp_tail->minute;
  tTm.tm_sec = udp_tail->second;
  tTm.tm_isdst = 0;
  auto timestamp_s = static_cast<double>(timegm(&tTm));
  // The sub second part of the timestamp
  const unsigned int timestamp_us = udp_tail->timestamp;
  uint64_t pkt_timestamp_ns =
      static_cast<uint64_t>(timestamp_s * 1e9) + timestamp_us * 1e3;
  // data
  size_t data_offset = 0;
  for (size_t i = 0; i < unit_column_num; i++) {
    int unit_id = udp_pkt->unit_start_id + i;
    auto column_unit = reinterpret_cast<const ColumnUnit5_2*>(
        udp_pkt->laser_data + data_offset);
    // update offset
    data_offset = data_offset + 1 + column_unit->return_num * 4;
    // check offset
    if (data_offset > data_size) {
      LOG(ERROR)
          << "Invalid packet, column unit data overflow, start unit id: " +
                 std::to_string(udp_pkt->unit_start_id) +
                 ", unit column number: " +
                 std::to_string(udp_pkt->unit_column_num) +
                 ", current unit id: " + std::to_string(unit_id);
      return false;
    }
    for (size_t j = 0; j < column_unit->return_num; j++) {
      auto& return_unit = column_unit->returns[j];
      if ((return_unit.distance & 0xffff) == 1) {
        continue;
      }
      float dist_in_m = return_unit.distance * distance_unit +
                        distance_offset_map_[udp_pkt->laser_id];
      float elevation = (365 - unit_id) * 0.3 + udp_pkt->laser_id * (0.3 / 4);
      Point p;
      p.timestamp = pkt_timestamp_ns / 1e9;
      p.laser_id = udp_pkt->laser_id;
      p.return_id = j;
      p.azimuth_encoder = udp_pkt->encoder;
      p.elevation_encoder = unit_id;
      p.frame_id = udp_pkt->frame_id;
      p.azimuth = azimuth;  // TODO: fix
      p.elevation = elevation;
      // for azimuth, y-axis is 0 angle.
      // for elevation, z-axis is 0 angle.
      // clockwise is positive direction
      float lxz = dist_in_m *
                  CosLookupDegree(g_azimuth_offset_map_a2[udp_pkt->laser_id]);
      float ly = -dist_in_m *
                 SinLookupDegree(g_azimuth_offset_map_a2[udp_pkt->laser_id]);
      float lx = lxz * SinLookupDegree(elevation);
      p.x = lx * CosLookupDegree(azimuth) + ly * SinLookupDegree(azimuth);
      p.y = -(-lx * SinLookupDegree(azimuth) + ly * CosLookupDegree(azimuth));
      p.z = lxz * CosLookupDegree(elevation);
      p.distance = dist_in_m;
      p.intensity = return_unit.reflectivity;
      p.elongation = return_unit.elongation;
      // for range image
      p.scan_column = udp_pkt->column_id * 4 + udp_pkt->laser_id;
      p.scan_row = unit_id;
      point_cloud->points.push_back(p);
    }
  }
  point_cloud->version = udp_pkt->header.minor_version;
  return false;
}

bool BLidarDriver::ParsePacket5_1(const uint8_t* data, size_t len,
                                  std::shared_ptr<PointCloud> point_cloud) {
  // check valid
  if (len != kLidarPacketSize) {
    return false;
  }
  auto udp_pkt = reinterpret_cast<const LidarPacket*>(data);

  struct tm tTm;
  // year since 1900
  tTm.tm_year = udp_pkt->tail.year;
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = udp_pkt->tail.month - 1;
  tTm.tm_mday = udp_pkt->tail.day;
  tTm.tm_hour = udp_pkt->tail.hour;
  tTm.tm_min = udp_pkt->tail.minute;
  tTm.tm_sec = udp_pkt->tail.second;
  tTm.tm_isdst = 0;
  auto timestamp_s = static_cast<double>(timegm(&tTm));
  // TODO(kewei) consider endianness, e.g. HsByteArrToUint(udp_pkt->timestamp)
  // The sub second part of the timestamp
  const unsigned int timestamp_us = udp_pkt->tail.timestamp;
  uint64_t pkt_timestamp_ns =
      static_cast<uint64_t>(timestamp_s * 1e9) + timestamp_us * 1e3;
  const double distance_unit = udp_pkt->header.chDisUnit / 1000.0;
  unsigned int laser_num = udp_pkt->header.chLaserNumber;
  uint8_t return_id = udp_pkt->header.returnId;
  // for each block
  for (int i = 0; i < udp_pkt->header.chBlockNumber; ++i) {
    CalcPointXYZIT(udp_pkt, i, return_id, laser_num, pkt_timestamp_ns,
                   distance_unit, point_cloud);
  }
  // update point_cloud info
  point_cloud->version = udp_pkt->header.protocalVersionMinor;
  point_cloud->max_return_num = udp_pkt->header.returnNum;
  point_cloud->timestamp = pkt_timestamp_ns / 1e9;
  // for all the units/points in a block
  point_cloud->udp_sequences.push_back(udp_pkt->tail.udpSequence);
  return true;
}

}  // namespace blidar
}  // namespace drivers
}  // namespace autox
