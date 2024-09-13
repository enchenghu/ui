/******************************************************************************
 * Copyright 2023 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#include "driver/xlidar_driver.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "utils/utils.h"

namespace autox {
namespace drivers {
namespace xlidar {

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding
typedef struct {
  uint8_t sop[2];         // 0xEEFF 2bytes
  uint8_t major_version;  // 0x04
  uint8_t minor_version;  // 0x02
  uint16_t data_size;
} PacketHeader;

// for protocol 4.1
typedef struct {
  uint16_t distance;  // *4mm, max distance: (2^16-1) * 4mm = 262.14m
  uint8_t intensity;  // intensity: 0 to 255%
  uint8_t reserved;
} LaserUnit4_1;

typedef struct {
  uint16_t azimuth;
  LaserUnit4_1 units[128];
} DataBlock4_1;

typedef struct {
  PacketHeader header;
  // data head
  unsigned char chLaserNumber;  // laser number: 0x80 (128)
  unsigned char chBlockNumber;  // block number: 0x02
  char chBlockType;     // 0x01: 1-128, 0x00: 129-256.(only for xlidar256)
  char chDisUnit;       // Distance unit, 6mm/5mm/4mm
  char echoNum;         // 0x01: single return ; 0x02: dual return
  char hasUdpSequence;  // 0x01: has UdpSequence in UDP packet
  // data blocks
  DataBlock4_1 blocks[2];
  // data tail
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
} LidarPacket4_1;

// for protocol 4.2
typedef struct {
  uint16_t distance;  // *4mm, max distance: (2^16-1) * 4mm = 262.14m
  uint8_t intensity;  // reflectivity: 0 to 255%
  uint8_t elongation;
} ReturnUnit4_2;

typedef struct {
  uint8_t return_num;
  ReturnUnit4_2 returns[0];
} LaserUnit4_2;

typedef struct {
  PacketHeader header;
  // data head
  uint16_t encoder;
  uint8_t section_type;
  uint8_t first_laser_id;
  uint8_t laser_step;
  uint8_t laser_num;
  uint8_t max_return_num;
  uint8_t return_mode;
  uint8_t distance_unit;
  uint8_t is_last_packet;
  uint8_t high_temperature_flag;
  uint8_t flag;
  uint16_t motor_speed;
  uint64_t timestamp_s;
  uint32_t timestamp_us;
  uint32_t udp_sequence;
  uint8_t reserved[10];
  // data
  uint8_t laser_data[0];
} LidarPacket4_2;

#pragma pack(pop)
constexpr unsigned int kLidarPacketSize4_1 = sizeof(LidarPacket4_1);  // 1068

static const float g_elevation_offset[] = {
    7.604,   7.452,   7.302,   7.154,   7.008,   6.864,   6.722,   6.582,
    6.444,   6.308,   6.174,   6.042,   5.912,   5.784,   5.658,   5.534,
    5.412,   5.292,   5.174,   5.058,   4.944,   4.832,   4.722,   4.614,
    4.508,   4.404,   4.302,   4.202,   4.104,   4.008,   3.914,   3.822,
    3.732,   3.644,   3.558,   3.474,   3.392,   3.312,   3.234,   3.158,
    3.084,   3.012,   2.942,   2.874,   2.808,   2.744,   2.682,   2.622,
    2.564,   2.508,   2.454,   2.402,   2.352,   2.304,   2.256,   2.208,
    2.16,    2.112,   2.064,   2.016,   1.968,   1.92,    1.872,   1.824,
    1.776,   1.728,   1.68,    1.632,   1.584,   1.536,   1.488,   1.44,
    1.392,   1.344,   1.296,   1.248,   1.2,     1.152,   1.104,   1.056,
    1.008,   0.96,    0.912,   0.864,   0.816,   0.768,   0.72,    0.672,
    0.624,   0.576,   0.528,   0.48,    0.432,   0.384,   0.336,   0.288,
    0.24,    0.192,   0.144,   0.096,   0.048,   0,       -0.048,  -0.096,
    -0.144,  -0.192,  -0.24,   -0.288,  -0.336,  -0.384,  -0.432,  -0.48,
    -0.528,  -0.576,  -0.624,  -0.672,  -0.72,   -0.768,  -0.816,  -0.864,
    -0.912,  -0.96,   -1.008,  -1.056,  -1.104,  -1.152,  -1.2,    -1.248,
    -1.296,  -1.344,  -1.392,  -1.44,   -1.488,  -1.536,  -1.584,  -1.632,
    -1.68,   -1.728,  -1.776,  -1.824,  -1.872,  -1.92,   -1.968,  -2.016,
    -2.064,  -2.112,  -2.16,   -2.208,  -2.256,  -2.304,  -2.352,  -2.4,
    -2.448,  -2.496,  -2.544,  -2.592,  -2.64,   -2.688,  -2.736,  -2.784,
    -2.832,  -2.88,   -2.928,  -2.976,  -3.024,  -3.072,  -3.122,  -3.174,
    -3.228,  -3.284,  -3.342,  -3.402,  -3.464,  -3.528,  -3.594,  -3.662,
    -3.732,  -3.804,  -3.878,  -3.954,  -4.032,  -4.112,  -4.194,  -4.278,
    -4.364,  -4.452,  -4.542,  -4.634,  -4.728,  -4.824,  -4.922,  -5.022,
    -5.124,  -5.228,  -5.334,  -5.442,  -5.552,  -5.664,  -5.778,  -5.894,
    -6.012,  -6.132,  -6.254,  -6.378,  -6.504,  -6.632,  -6.762,  -6.894,
    -7.028,  -7.164,  -7.302,  -7.442,  -7.584,  -7.728,  -7.874,  -8.022,
    -8.172,  -8.324,  -8.478,  -8.634,  -8.792,  -8.952,  -9.114,  -9.278,
    -9.444,  -9.612,  -9.782,  -9.954,  -10.128, -10.304, -10.482, -10.662,
    -10.844, -11.028, -11.214, -11.402, -11.592, -11.784, -11.978, -12.174,
    -12.372, -12.572, -12.774, -12.978, -13.184, -13.392, -13.602, -13.814,
    -14.028, -14.244, -14.462, -14.682, -14.904, -15.128, -15.354, -15.582};

static const float g_azimuth_offset[] = {
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572,
    3.2, -3.2, 2.2857, -2.2858, 1.3714, -1.3715, 0.4571, -0.4572};

inline double normalize_angle_deg(double a) {
  while (a > 360) {
    a -= 360;
  }
  while (a < 0) {
    a += 360;
  }
  return a;
}

XLidarDriver::XLidarDriver() {
  // time offset
  for (size_t i = 0; i < kLaserNum; ++i) {
    laser_time_offset_[i] = 10.0f + 2.31f + 2.06f * i;
  }
  // init sin/cos table
  for (size_t i = 0; i < kRotationMaxUnits; ++i) {
    double rotation = 1.0 / kSinCosTableResolution * i * kDegreeToRadian;
    cos_lookup_table_[i] = cos(rotation);
    sin_lookup_table_[i] = sin(rotation);
  }
  // init packet buffer
  for (size_t i = 0; i < max_packet_buffer_size_; i++) {
    auto packet = std::make_shared<Packet>();
    packet->point_cloud = std::make_shared<PointCloud>();
    ClearPacket(*packet);
    packet_buffer_.push_back(packet);
  }
  // init point cloud buffer
  for (size_t i = 0; i < max_point_cloud_buffer_size_; i++) {
    auto point_cloud = std::make_shared<PointCloud>();
    ClearPointcloud(point_cloud);
    point_cloud_buffer_.push_back(point_cloud);
  }
  // init driver flag
  for (int i = 0; i < kSectionNum; i++) {
    last_azimuth_[i] = 0;
    first_check_flag_[i] = false;
  }
  // init lidar intrinsics
  std::vector<double> zero_offset(256, 0.0);
  std::vector<double> init_elevation_offset(256, 0);
  std::vector<double> init_azimuth_offset(256, 0);
  for (int i = 0; i < kLaserNum; ++i) {
    init_elevation_offset[i] = g_elevation_offset[i];
    init_azimuth_offset[i] = g_azimuth_offset[i];
  }
  SetIntrinsics(init_elevation_offset, init_azimuth_offset, zero_offset,
                zero_offset, zero_offset, zero_offset);
  ResetDriver();
}

XLidarDriver::~XLidarDriver() {
  if (use_multiple_threads_) {
    exit_ = true;
    packet_buffer_not_full_.notify_all();
    assembler_thread_->join();
    for (int i = 0; i < parser_thread_number_; i++) {
      parser_threads_[i]->join();
    }
  }
}

bool XLidarDriver::SetMultiThreadMode(int thread_mum) {
  index_f_single_thread_ = 0;
  if (use_multiple_threads_) {
    exit_ = true;
    packet_buffer_not_full_.notify_all();
    assembler_thread_->join();
    for (int i = 0; i < parser_thread_number_; i++) {
      parser_threads_[i]->join();
    }
    parser_threads_.clear();
    exit_ = false;
    use_multiple_threads_ = false;
  }
  if (thread_mum < 2) {
    return false;
  }
  // for multiple threads
  parser_thread_number_ = thread_mum;
  use_multiple_threads_ = true;
  Debug("[driver] parser thread number:" +
        std::to_string(parser_thread_number_));
  // create a assembler thread
  assembler_thread_ =
      std::make_unique<std::thread>([this]() { AssemblerLoop(); });
  pthread_setname_np(assembler_thread_->native_handle(), "pv/assemble");
  // create multiple parser threads
  for (int i = 0; i < parser_thread_number_; i++) {
    auto t = std::make_unique<std::thread>([this]() { ParserLoop(); });
    std::string name = "pv/parse-" + std::to_string(i);
    parser_threads_.push_back(std::move(t));
    pthread_setname_np(parser_threads_[i]->native_handle(), name.c_str());
  }
  return true;
}

void XLidarDriver::SetPointCloudCallback(PointCloudCallbackType cb) {
  point_cloud_callback_ = cb;
};

void XLidarDriver::ResetDriver() {
  cur_packet_index_ = 0;
  point_cloud_ = point_cloud_buffer_[point_cloud_buffer_index_];
  point_cloud_buffer_index_ =
      (point_cloud_buffer_index_ + 1) % point_cloud_buffer_.size();
  ClearPointcloud(point_cloud_);
  accumulate_azimuth_ = 0;
  for (int i = 0; i < kSectionNum; i++) {
    first_check_flag_[i] = false;
  }
}

void XLidarDriver::ClearPacket(Packet& packet) {
  packet.len = 0;
  packet.parsed = false;
  packet.point_cloud->points.clear();
  packet.point_cloud->points.reserve(512);
  for (int i = 0; i < kSectionNum; i++) {
    packet.point_cloud->udp_sequences[i].clear();
  }
  packet.point_cloud->udp_packet_number = 0;
  packet.offset_in_pcap = 0;
  packet.index_in_pcap = 0;
}

void XLidarDriver::ClearPointcloud(std::shared_ptr<PointCloud> point_cloud) {
  point_cloud->points.clear();
  point_cloud->points.reserve(5120000);
  for (int i = 0; i < kSectionNum; i++) {
    point_cloud->udp_sequences[i].clear();
    point_cloud->udp_sequences[i].reserve(20000);
  }
  point_cloud->udp_packet_number = 0;
}
bool XLidarDriver::ParseLidarPacket(const uint8_t* data, size_t len) {
  // check valid
  if (len > 1500 || len < 10) {
    Debug("Packet len invalid.");
    return false;
  }
  if (use_multiple_threads_) {
    auto packet = packet_buffer_[packet_buffer_index_];
    packet_buffer_index_ = (packet_buffer_index_ + 1) % packet_buffer_.size();
    ClearPacket(*packet);
    memcpy(packet->data, data, len);
    packet->len = len;
    packet->index_in_pcap = cur_packet_index_;
    packet->offset_in_pcap = cur_packet_offset_;
    {
      std::unique_lock<std::mutex> lk(packet_buffer_mutex_);
      used_packet_count_++;
      packet_buffer_not_full_.wait(lk, [this] {
        return exit_ || (used_packet_count_ < packet_buffer_.size() - 3);
      });
      if (exit_) {
        return false;
      }
      std::unique_lock<std::mutex> lk2(parser_buffer_mutex_);
      parser_buffer_.push_back(packet);
    }
  } else {
    // parse udp packet
    if (point_cloud_->points.empty()) {
      point_cloud_->begin_index_in_pacp = cur_packet_index_;
    }
    if (!ParsePacket(data, len, point_cloud_)) {
      return false;
    }
    point_cloud_->udp_packet_number++;
    // check if a pointcloud frame assembled
    if (!point_cloud_->points.empty()) {
      double azimuth = point_cloud_->points.back().encoder;
      int section_type = point_cloud_->points.back().section;
      if (CheckAngle(azimuth, section_type) ||
          point_cloud_->udp_packet_number > 22000) {
        if (point_cloud_->points.size() > 0 && point_cloud_callback_) {
          point_cloud_->end_index_in_pacp = cur_packet_index_;
          point_cloud_->offset_in_pcap = cur_packet_offset_;
          point_cloud_->timestamp = point_cloud_->points[0].timestamp;
          point_cloud_callback_(point_cloud_, index_f_single_thread_++);
        }
        // reset
        ResetDriver();
      }
      last_azimuth_[section_type] = azimuth;
    }
  }
  return true;
}

bool XLidarDriver::ParsePacket(const uint8_t* data, size_t len,
                               std::shared_ptr<PointCloud> point_cloud) {
  // check header
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xee || header->sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Packet Header Error: " << std::hex << (0xFF & header->sop[0])
       << (0xFF & header->sop[1]) << std::dec;
    Debug(ss.str());
    return false;
  }
  if (header->major_version == 0x04 && header->minor_version == 0x01) {
    return ParsePacket4_1(data, len, point_cloud);
  } else if (header->major_version == 0x04 && header->minor_version == 0x02) {
    return ParsePacket4_2(data, len, point_cloud);
  } else {
    Debug("Invalid packet, packet version: " +
          std::to_string(header->major_version) + "." +
          std::to_string(header->minor_version) + ". new firmware?");
  }
  return false;
}

bool XLidarDriver::ParsePacket4_1(const uint8_t* data, size_t len,
                                  std::shared_ptr<PointCloud> point_cloud) {
  if (len != kLidarPacketSize4_1) {
    Debug("Invalid packet, packet size need be: " +
          std::to_string(kLidarPacketSize4_1) + ", but " + std::to_string(len));
    return false;
  }
  auto udp_pkt = reinterpret_cast<const LidarPacket4_1*>(data);
  uint8_t max_return_num = udp_pkt->echoNum;
  // '\x01' single return
  bool is_dual_return = (udp_pkt->echoNum == '\x02');
  struct tm tTm;
  // year since 1900
  tTm.tm_year = udp_pkt->year;
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = udp_pkt->month - 1;
  tTm.tm_mday = udp_pkt->day;
  tTm.tm_hour = udp_pkt->hour;
  tTm.tm_min = udp_pkt->minute;
  tTm.tm_sec = udp_pkt->second;
  tTm.tm_isdst = 0;
  auto timestamp_s = static_cast<double>(timegm(&tTm));
  // TODO(kewei) consider endianness, e.g. HsByteArrToUint(udp_pkt->timestamp)
  // The sub second part of the timestamp
  const unsigned int timestamp_us = udp_pkt->timestamp;
  uint64_t pkt_timestamp_ns =
      static_cast<uint64_t>(timestamp_s * 1e9) + timestamp_us * 1e3;
  const double distance_unit = udp_pkt->chDisUnit / 1000.0;
  unsigned int laser_num = udp_pkt->chLaserNumber;
  assert(udp_pkt->chLaserNumber == 128 && udp_pkt->chBlockNumber == 2);
  unsigned int block_type = udp_pkt->chBlockType;
  if (block_type > kSectionNum - 1) {
    Debug("Error section number: " + std::to_string(block_type));
    return false;
  }
  // for each block
  for (int blockid = 0; blockid < udp_pkt->chBlockNumber; blockid++) {
    if (is_dual_return && blockid % 2 == 1) {
      // The second return will be parsed together with the first return,
      // In the dual return mode, We assume that the block number is always
      // even as we have observed.
      continue;
    }
    const auto& block = udp_pkt->blocks[blockid];
    float encoder = block.azimuth * azimuth_unit_;
    // for all the units/points in a block
    std::vector<float> distance;
    std::vector<uint8_t> intensity;
    distance.reserve(2);
    intensity.reserve(2);
    for (unsigned int i = 0; i < laser_num; ++i) {
      const auto& unit = block.units[i];
      unsigned int laser_id = i * 2 + block_type;
      distance.clear();
      intensity.clear();
      distance.push_back(unit.distance * distance_unit);
      intensity.push_back(unit.intensity & 0xff);
      if (is_dual_return) {
        const auto& second_unit = udp_pkt->blocks[blockid + 1].units[i];
        if (unit.distance != second_unit.distance) {
          distance.push_back(second_unit.distance * distance_unit);
          intensity.push_back(second_unit.intensity & 0xff);
        }
      }
      for (size_t j = 0; j < distance.size(); j++) {
        float dist = distance[j] + distance_offset_[laser_id];
        float corrected_azimuth = azimuth_offset_[laser_id] + encoder;
        // normalize azimuth
        corrected_azimuth = normalize_angle_deg(corrected_azimuth);
        if (!CheckValid(corrected_azimuth, dist)) {
          continue;
        }
        //
        float xy_dist = dist * CosLookupDegree(elevation_offset_[laser_id]);
        float lx = origin_x_offset_[i] +
                   xy_dist * SinLookupDegree(azimuth_offset_[laser_id]);
        float ly = origin_y_offset_[i] +
                   xy_dist * CosLookupDegree(azimuth_offset_[laser_id]);
        float lz = origin_z_offset_[i] +
                   dist * SinLookupDegree(elevation_offset_[laser_id]);
        Point p;
        uint64_t offset =
            (static_cast<uint64_t>(laser_time_offset_[laser_id]) * 1000);
        p.timestamp = (pkt_timestamp_ns - offset) / 1e9;
        p.laser_id = laser_id;
        p.return_id = j;
        p.encoder = encoder;
        p.distance = dist;
        p.intensity = intensity[j];
        p.elongation = 0;
        p.azimuth = corrected_azimuth;
        p.elevation = elevation_offset_[laser_id];
        p.azimuth_code = block.azimuth;
        // TODO(all), it's differnet from hesai reconstruction model.
        // in xlidar, azimuth is measured in coordinate system,
        // anticlockwise is positive direction, y-axis is 0 angle.
        p.x = lx * CosLookupDegree(encoder) + ly * SinLookupDegree(encoder);
        p.y = -lx * SinLookupDegree(encoder) + ly * CosLookupDegree(encoder);
        p.z = lz;
        p.section = block_type;
        //

        point_cloud->points.push_back(p);
      }
    }
  }
  // update point_cloud info
  point_cloud->version = udp_pkt->header.minor_version;
  point_cloud->max_return_num = max_return_num;
  point_cloud->timestamp = pkt_timestamp_ns / 1e9;
  point_cloud->udp_sequences[block_type].push_back(udp_pkt->udpSequence);
  return true;
}

bool XLidarDriver::ParsePacket4_2(const uint8_t* data, size_t len,
                                  std::shared_ptr<PointCloud> point_cloud) {
  auto udp_pkt = reinterpret_cast<const LidarPacket4_2*>(data);
  // check
  if (udp_pkt->header.data_size + 8 != len) {
    Debug("Invalid packet, packet size need be data size + 8, packet size:" +
          std::to_string(len) +
          ", data size:" + std::to_string(udp_pkt->header.data_size));
    return false;
  }
  if (data[len - 2] != 0x0d || data[len - 1] != 0x0a) {
    Debug("Invalid packet, tail need be 0x0d0a.");
    return false;
  }
  uint64_t pkt_timestamp_ns =
      udp_pkt->timestamp_s * 1e9 + udp_pkt->timestamp_us * 1e3;
  float distance_unit = udp_pkt->distance_unit / 1000.0;
  float encoder = udp_pkt->encoder * azimuth_unit_;
  size_t laser_num = udp_pkt->laser_num;
  assert(udp_pkt->laser_step == 2);
  size_t laser_data_size = udp_pkt->header.data_size - 40;
  int section_type = (int)udp_pkt->section_type;
  if (section_type > kSectionNum - 1) {
    Debug("Error section number: " + std::to_string(section_type));
    return false;
  }
  // parse point for each laser unit
  size_t data_offset = 0;
  for (size_t i = 0; i < laser_num; ++i) {
    size_t laser_id = udp_pkt->first_laser_id + udp_pkt->laser_step * i;
    auto laser_unit = reinterpret_cast<const LaserUnit4_2*>(
        udp_pkt->laser_data + data_offset);
    // update offset
    data_offset = data_offset + 1 + laser_unit->return_num * 4;
    // check offset
    if (data_offset > laser_data_size) {
      Debug("Invalid packet, laser unit data overflow, laser id: " +
            std::to_string(laser_id));
      return false;
    }
    for (size_t j = 0; j < laser_unit->return_num; j++) {
      auto& return_unit = laser_unit->returns[j];
      if (return_unit.distance & 0xfffff == 1) {
        continue;
      }
      float dist =
          return_unit.distance * distance_unit + distance_offset_[laser_id];
      float corrected_azimuth = encoder + azimuth_offset_[laser_id];
      // normalize azimuth
      corrected_azimuth = normalize_angle_deg(corrected_azimuth);
      if (!CheckValid(corrected_azimuth, dist)) {
        continue;
      }
      //
      double xy_dist = dist * elevation_offset_cos_[laser_id];
      float lx = origin_x_offset_[i] + xy_dist * azimuth_offset_sin_[laser_id];
      float ly = origin_y_offset_[i] + xy_dist * azimuth_offset_cos_[laser_id];
      float lz = origin_z_offset_[i] + dist * elevation_offset_sin_[laser_id];
      Point p;
      uint64_t offset =
          (static_cast<uint64_t>(laser_time_offset_[laser_id]) * 1000);
      p.timestamp = (pkt_timestamp_ns + offset) / 1e9;
      p.laser_id = laser_id;
      p.return_id = j;
      p.encoder = encoder;
      p.distance = dist;
      p.intensity = return_unit.intensity;
      p.elongation = return_unit.elongation;
      p.azimuth = corrected_azimuth;
      p.elevation = elevation_offset_[laser_id];
      p.section = section_type;
      p.azimuth_code = udp_pkt->encoder;
      // TODO(all), it's differnet from hesai reconstruction model.
      // in xlidar, azimuth is measured in coordinate system,
      // anticlockwise is positive direction, y-axis is 0 angle.
      p.x = lx * CosLookupDegree(encoder) + ly * SinLookupDegree(encoder);
      p.y = -lx * SinLookupDegree(encoder) + ly * CosLookupDegree(encoder);
      p.z = lz;
      //
      point_cloud->points.push_back(p);
    }
  }
  // update point_cloud info
  point_cloud->version = udp_pkt->header.minor_version;
  point_cloud->max_return_num = udp_pkt->max_return_num;
  point_cloud->timestamp = pkt_timestamp_ns / 1e9;
  point_cloud->udp_sequences[section_type].push_back(udp_pkt->udp_sequence);
  return true;
}

float XLidarDriver::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return cos_lookup_table_[int(angle_in_deg * kSinCosTableResolution)];
}

float XLidarDriver::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return sin_lookup_table_[int(angle_in_deg * kSinCosTableResolution)];
}

bool XLidarDriver::CheckAngle(float cur_azimuth, int section) {
  if (!first_check_flag_[section]) {
    last_azimuth_[section] = cur_azimuth;
    first_check_flag_[section] = true;
    return false;
  }
  float last_azimuth = last_azimuth_[section];
  float azimuth_gap = 0;
  constexpr float kAzimuthThre = 2;  // 2 degree
  if (cur_azimuth - last_azimuth > 0 &&
      cur_azimuth - last_azimuth < kAzimuthThre) {
    azimuth_sign_ = 1;  // clockwise
  } else if (last_azimuth - cur_azimuth > 0 &&
             last_azimuth - cur_azimuth < kAzimuthThre) {
    azimuth_sign_ = -1;  // countclockwise
  }
  if (azimuth_sign_ * (last_azimuth - cur_azimuth) > 0) {
    azimuth_gap = 360 + azimuth_sign_ * (cur_azimuth - last_azimuth);
  } else {
    azimuth_gap = azimuth_sign_ * (cur_azimuth - last_azimuth);
  }
  accumulate_azimuth_ += azimuth_gap;
  if (fabs(last_azimuth - cur_azimuth) > 0.0001) {
    if ((azimuth_sign_ * (last_azimuth - cur_azimuth) > 0 &&
         (azimuth_sign_ * (start_angle_ - cur_azimuth) <= 0 ||
          azimuth_sign_ * (start_angle_ - last_azimuth) > 0)) ||
        (azimuth_sign_ * (last_azimuth - start_angle_) < 0 &&
         azimuth_sign_ * (start_angle_ - cur_azimuth) <= 0)) {
      constexpr float kMinPublishFOV = 30;  // 30degree
      return accumulate_azimuth_ > kMinPublishFOV;
    }
  }
  return false;
}

bool XLidarDriver::CheckValid(float azimuth, float distance) {
  //  if (distance < min_distance_ || distance > max_distance_) {
  //    return false;
  //  }
  if (distance < min_distance_) {
    return false;
  }
  if (hfov_start_ < hfov_end_) {
    if (azimuth < hfov_start_ || azimuth > hfov_end_) {
      return false;
    }
  } else {
    if (azimuth > hfov_end_ && azimuth < hfov_start_) {
      return false;
    }
  }
  return true;
}

void XLidarDriver::ParserLoop() {
  while (!exit_) {
    std::shared_ptr<Packet> packet = nullptr;
    // get unparsed packet
    {
      std::lock_guard<std::mutex> lk(parser_buffer_mutex_);
      if (!parser_buffer_.empty()) {
        packet = parser_buffer_.front();
        parser_buffer_.pop_front();
      }
    }
    if (packet == nullptr) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
      continue;
    }
    // parse udp packet
    packet->valid = ParsePacket(packet->data, packet->len, packet->point_cloud);
    packet->parsed = true;
  }
}

void XLidarDriver::AssemblerLoop() {
  long long index_f = 0;
  while (!exit_) {
    std::shared_ptr<Packet> packet = nullptr;
    {
      std::lock_guard<std::mutex> lk(packet_buffer_mutex_);
      if (packet_buffer_[assembling_index_]->parsed) {
        packet = packet_buffer_[assembling_index_];
        assembling_index_ = (assembling_index_ + 1) % packet_buffer_.size();
        used_packet_count_--;
        packet_buffer_not_full_.notify_one();
      }
    }
    if (packet == nullptr) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);
      continue;
    }
    // reset parsed flag
    packet->parsed = false;
    // check valid
    if (!packet->valid) {
      continue;
    }
    // assemble temporary point cloud
    auto pc = packet->point_cloud;
    // update point cloud info
    if (point_cloud_->udp_packet_number == 0) {
      point_cloud_->timestamp = pc->timestamp;
      point_cloud_->version = pc->version;
      point_cloud_->max_return_num = pc->max_return_num;
      point_cloud_->begin_index_in_pacp = packet->index_in_pcap;
    }
    point_cloud_->udp_packet_number++;
    if (pc->points.size() == 0) {
      continue;
    }
    // copy data into point_cloud_
    point_cloud_->points.insert(point_cloud_->points.end(),
                                std::make_move_iterator(pc->points.begin()),
                                std::make_move_iterator(pc->points.end()));
    for (int i = 0; i < kSectionNum; i++) {
      point_cloud_->udp_sequences[i].insert(
          point_cloud_->udp_sequences[i].end(), pc->udp_sequences[i].begin(),
          pc->udp_sequences[i].end());
    }

    // check if a pointcloud frame assembled
    double azimuth = point_cloud_->points.back().encoder;
    int section_type = point_cloud_->points.back().section;
    if (CheckAngle(azimuth, section_type) ||
        point_cloud_->udp_packet_number > 22000) {
      if (point_cloud_->points.size() > 0 && point_cloud_callback_) {
        point_cloud_->end_index_in_pacp = packet->index_in_pcap;
        point_cloud_->offset_in_pcap = packet->offset_in_pcap;
        point_cloud_->timestamp = point_cloud_->points[0].timestamp;
        point_cloud_callback_(point_cloud_, index_f++);
      }
      // clear
      ResetDriver();
    }
    last_azimuth_[section_type] = azimuth;
  }
}

bool XLidarDriver::SetIntrinsics(const std::vector<double>& elevation_offset,
                                 const std::vector<double>& azimuth_offset,
                                 const std::vector<double>& distance_offset,
                                 const std::vector<double>& origin_x_offset,
                                 const std::vector<double>& origin_y_offset,
                                 const std::vector<double>& origin_z_offset) {
  for (int i = 0; i < kLaserNum; i++) {
    elevation_offset_[i] = elevation_offset[i];
    elevation_offset_cos_[i] = CosLookupDegree(elevation_offset_[i]);
    elevation_offset_sin_[i] = SinLookupDegree(elevation_offset_[i]);
    azimuth_offset_[i] = azimuth_offset[i];
    azimuth_offset_cos_[i] = CosLookupDegree(azimuth_offset_[i]);
    azimuth_offset_sin_[i] = SinLookupDegree(azimuth_offset_[i]);
    distance_offset_[i] = distance_offset[i];
    origin_x_offset_[i] = origin_x_offset[i];
    origin_y_offset_[i] = origin_y_offset[i];
    origin_z_offset_[i] = origin_z_offset[i];
  }

  return true;
}

bool XLidarDriver::GetIntrinsics(std::vector<double>& elevation_offset,
                                 std::vector<double>& azimuth_offset,
                                 std::vector<double>& distance_offset,
                                 std::vector<double>& origin_x_offset,
                                 std::vector<double>& origin_y_offset,
                                 std::vector<double>& origin_z_offset) {
  for (int i = 0; i < kLaserNum; i++) {
    elevation_offset.push_back(elevation_offset_[i]);
    azimuth_offset.push_back(azimuth_offset_[i]);
    distance_offset.push_back(distance_offset_[i]);
    origin_x_offset.push_back(origin_x_offset_[i]);
    origin_y_offset.push_back(origin_y_offset_[i]);
    origin_z_offset.push_back(origin_z_offset_[i]);
  }
  return true;
}

}  // namespace xlidar
}  // namespace drivers
}  // namespace autox
