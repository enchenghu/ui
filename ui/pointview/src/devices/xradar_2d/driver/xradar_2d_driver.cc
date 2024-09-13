/******************************************************************************
 * Copyright 2023 AutoX Inc. All Rights Reserved.
 *****************************************************************************/

#include "driver/xradar_2d_driver.h"

#include <sys/types.h>

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "utils/utils.h"

constexpr int kSinCosTableResolution = 1000;
constexpr unsigned int kRotationMaxUnits = 360 * kSinCosTableResolution + 1;
constexpr double kDegreeToRadian = M_PI / 180.;

namespace autox {
namespace drivers {
namespace xradar_2d {

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding
typedef struct {
  uint8_t sop[2];         // message type
  uint8_t major_version;  //
  uint8_t minor_version;  //
  uint32_t packet_len;
} PacketHeader;

// for protocol 4.1
typedef struct {
  uint16_t range;   // *4mm, max distance: (2^16-1) * 4mm = 262.14m
  int16_t doppler;  // intensity: 0 to 255%
  int8_t azimuth;
  uint8_t log_power;
} DataUnit;

typedef struct {
  PacketHeader header;
  uint16_t sec_msb;
  uint32_t seconds;
  uint32_t us;
  uint32_t udp_sequence;
  uint8_t last_packet;
  uint8_t detect_type;
  float range_resolution;
  float doppler_resolution;
  float azimuth_resolution;
  uint8_t reserved[2];
  DataUnit data[0];
} RadarPacket;

#pragma pack(pop)

inline double normalize_angle_deg(double a) {
  while (a > 360) {
    a -= 360;
  }
  while (a < 0) {
    a += 360;
  }
  return a;
}

XRadar2DDriver::XRadar2DDriver() {
  // init sin/cos table
  cos_lookup_table_.resize(kRotationMaxUnits);
  sin_lookup_table_.resize(kRotationMaxUnits);
  for (size_t i = 0; i < kRotationMaxUnits; ++i) {
    double rotation = 1.0 / kSinCosTableResolution * i * kDegreeToRadian;
    cos_lookup_table_[i] = cos(rotation);
    sin_lookup_table_[i] = sin(rotation);
  }
  // init packet buffer
  for (size_t i = 0; i < max_packet_buffer_size_; i++) {
    auto packet = std::make_shared<Packet>();
    packet->point_cloud = std::make_shared<PointCloud>();
    clearPacket(*packet);
    packet_buffer_.push_back(packet);
  }
  // init point cloud buffer
  for (size_t i = 0; i < max_point_cloud_buffer_size_; i++) {
    auto point_cloud = std::make_shared<PointCloud>();
    clearPointcloud(point_cloud);
    point_cloud_buffer_.push_back(point_cloud);
  }
  is_last_frame_.resize(1, false);
  resetDriver();
}

XRadar2DDriver::~XRadar2DDriver() {}

void XRadar2DDriver::setPointCloudCallback(PointCloudCallbackType cb) {
  point_cloud_callback_ = cb;
};

void XRadar2DDriver::resetDriver() {
  cur_packet_index_ = 0;
  point_cloud_ = point_cloud_buffer_[point_cloud_buffer_index_];
  point_cloud_buffer_index_ =
      (point_cloud_buffer_index_ + 1) % point_cloud_buffer_.size();
  clearPointcloud(point_cloud_);
  accumulate_azimuth_ = 0;
  for (int i = 0; i < is_last_frame_.size(); i++) {
    is_last_frame_[i] = false;
  }
}

void XRadar2DDriver::clearPacket(Packet& packet) {
  packet.len = 0;
  packet.parsed = false;
  packet.point_cloud->points.clear();
  packet.point_cloud->points.reserve(512);
  packet.point_cloud->udp_sequences.clear();
  packet.point_cloud->udp_packet_number;
  packet.offset_in_pcap = 0;
  packet.index_in_pcap = 0;
}

void XRadar2DDriver::clearPointcloud(std::shared_ptr<PointCloud> point_cloud) {
  point_cloud->points.clear();
  point_cloud->points.reserve(5120000);
  point_cloud->udp_sequences.clear();
  point_cloud->udp_sequences.reserve(50000);
  point_cloud->udp_packet_number = 0;
}
bool XRadar2DDriver::parseRadarPacket(const uint8_t* data, size_t len) {
  // check valid
  if (len > 1500 || len < 10) {
    return false;
  }
  // parse udp packet
  if (point_cloud_->points.empty()) {
    point_cloud_->begin_index_in_pacp = cur_packet_index_;
  }
  if (!parsePacket(data, len, point_cloud_)) {
    return false;
  }
  Debug("parse successfully");
  point_cloud_->udp_packet_number++;
  auto packet = reinterpret_cast<const RadarPacket*>(data);
  if (packet->last_packet == 1) {
    Debug("last frame");
    is_last_frame_[packet->detect_type] = true;
    bool last_frame = true;
    for (int i = 0; i < is_last_frame_.size(); i++) {
      if (is_last_frame_[i] == false) {
        last_frame = false;
        break;
      }
    }
    if (last_frame) {
      point_cloud_->end_index_in_pacp = cur_packet_index_;
      point_cloud_->offset_in_pcap = cur_packet_offset_;
      // measurement timstamp
      point_cloud_->timestamp = point_cloud_->points[0].timestamp;
      // resolution info
      point_cloud_->range_resolution = packet->range_resolution;
      point_cloud_->azimuth_resolution = packet->azimuth_resolution;
      // callback
      point_cloud_callback_(point_cloud_, index_f_single_thread_++);
      // reset
      resetDriver();
    }
  }

  return true;
}

bool XRadar2DDriver::parsePacket(const uint8_t* data, size_t len,
                                 std::shared_ptr<PointCloud> point_cloud) {
  // check header
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xa5 || header->sop[1] != 0xa5) {
    std::stringstream ss;
    ss << "Packet Header Error: " << std::hex << (0xFF & header->sop[0])
       << (0xFF & header->sop[1]) << std::dec;
    Debug(ss.str());
    return false;
  }
  if (header->major_version == 0x00 && header->minor_version == 0x00) {
    return parsePacket0_0(data, len, point_cloud);
  } else {
    Debug("Invalid packet, packet version: " +
          std::to_string(header->major_version) + "." +
          std::to_string(header->minor_version) + ". new firmware?");
  }
  return false;
}

bool XRadar2DDriver::parsePacket0_0(const uint8_t* data, size_t len,
                                    std::shared_ptr<PointCloud> point_cloud) {
  auto udp_pkt = reinterpret_cast<const RadarPacket*>(data);
  if (len != udp_pkt->header.packet_len) {
    Debug("len is wrong. udp len: " + std::to_string(len) +
          ". packet len: " + std::to_string(udp_pkt->header.packet_len));
    return false;
  }
  // parse ptp timestamp
  uint64_t seconds = (udp_pkt->sec_msb << 32) + udp_pkt->seconds;
  float ptp_timestamp_s = seconds + udp_pkt->us * 1e-6;
  int point_num = (udp_pkt->header.packet_len - 38) / 6;
  // for each block
  for (int blockid = 0; blockid < point_num; blockid++) {
    const auto& block = udp_pkt->data[blockid];
    float dist = block.range * udp_pkt->range_resolution;
    float doppler = block.doppler * udp_pkt->doppler_resolution;
    float azimuth = block.azimuth * udp_pkt->azimuth_resolution;

    // normalize azimuth
    azimuth = normalize_angle_deg(azimuth);
    if (!checkValid(azimuth, dist)) {
      continue;
    }
    //
    float lx = dist * SinLookupDegree(azimuth);
    float ly = dist * CosLookupDegree(azimuth);
    Point p;
    p.range_code = block.range;
    p.doppler_code = block.doppler;
    p.azimuth_code = block.azimuth;
    p.log_power = block.log_power;
    p.timestamp = ptp_timestamp_s;
    p.distance = dist;
    p.detect_type = udp_pkt->detect_type;
    p.x = lx;
    p.y = ly;
    p.z = 0;
    p.azimuth = azimuth;
    p.doppler = doppler;
    point_cloud->points.push_back(p);
  }
  // update point_cloud info
  point_cloud->version = udp_pkt->header.minor_version;
  point_cloud->timestamp = ptp_timestamp_s;
  point_cloud->udp_sequences.push_back(udp_pkt->udp_sequence);
  return true;
}

float XRadar2DDriver::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return cos_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

float XRadar2DDriver::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return sin_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

bool XRadar2DDriver::checkValid(float azimuth, float distance) {
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
}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox
