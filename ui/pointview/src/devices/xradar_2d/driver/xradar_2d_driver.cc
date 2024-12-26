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
struct PacketHeader {
  uint8_t sop[2];         // message type
  uint8_t major_version;  //
  uint8_t minor_version;  //
  uint32_t packet_len;
};

struct Timestamp {
  uint16_t sec_msb;
  uint32_t seconds;
  uint32_t nano_second;
};

struct Resolution {
  float range_resolution;
  float doppler_resolution;
  float azimuth_resolution;
};

struct DataUnit {
  uint16_t range;
  int16_t doppler;
  int8_t azimuth;
  uint8_t log_power;
};

typedef struct {
  PacketHeader header;
  Timestamp timestamp;
  uint32_t udp_sequence;
  uint8_t last_packet;
  uint8_t detect_type;
  Resolution resolution;
  uint8_t reserved[2];
  DataUnit data[0];
} RadarPacket;

typedef struct {
  PacketHeader header;
  Timestamp timestamp;
  uint32_t udp_sequence;
  uint8_t last_packet;
  uint8_t detect_type;
  Resolution resolution;
  uint8_t detect_number;
  uint8_t reserved[1];
  DataUnit data[0];
} RadarPacket1_0;

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
  // init point cloud
  InitBufferCloud(1);
  ResetDriver();
}

XRadar2DDriver::~XRadar2DDriver() {}

void XRadar2DDriver::SetPointCloudCallback(PointCloudCallbackType cb) {
  point_cloud_callback_ = cb;
};

void XRadar2DDriver::ResetDriver() {
  cur_packet_index_ = 0;
  udp_packet_number_ = 0;
  for (int i = 0; i < point_cloud_buffer_.size(); i++) {
    ClearPointcloud(point_cloud_buffer_[i]);
  }
}

void XRadar2DDriver::ClearPointcloud(PointCloudPtr point_cloud) {
  point_cloud->points.clear();
  point_cloud->points.reserve(51200);
  point_cloud->udp_sequences.clear();
  point_cloud->udp_sequences.reserve(100);
  point_cloud->udp_packet_number = 0;
}

bool XRadar2DDriver::ParseRadarPacket(const uint8_t* data, size_t len) {
  // check valid
  if (len > 1500 || len < 10) {
    return false;
  }
  // parse udp packet
  if (!ParsePacket(data, len)) {
    return false;
  }
  // LOG(INFO) << "parse successfully";
  udp_packet_number_++;
  auto packet = reinterpret_cast<const RadarPacket1_0*>(data);
  if (packet->last_packet == 1) {
    // LOG(INFO) << "receive last frame.";
    auto publish_cloud = std::make_shared<PointCloud>();
    publish_cloud->udp_packet_number = udp_packet_number_;
    publish_cloud->begin_index_in_pacp =
        point_cloud_buffer_[0]->begin_index_in_pacp;
    for (size_t i = 0; i < point_cloud_buffer_.size(); i++) {
      auto pc = point_cloud_buffer_[i];
      publish_cloud->udp_sequences.insert(publish_cloud->udp_sequences.end(),
                                          pc->udp_sequences.begin(),
                                          pc->udp_sequences.end());
      publish_cloud->points.insert(publish_cloud->points.end(),
                                   pc->points.begin(), pc->points.end());
      publish_cloud->begin_index_in_pacp =
          std::min(publish_cloud->begin_index_in_pacp, pc->begin_index_in_pacp);
      // resolution info
      publish_cloud->range_resolution.push_back(pc->range_resolution[0]);
      publish_cloud->azimuth_resolution.push_back(pc->azimuth_resolution[0]);
      // version
      if(pc->points.size() > 0){
        publish_cloud->major_version = point_cloud_buffer_[i]->major_version;
        publish_cloud->minor_version = point_cloud_buffer_[i]->minor_version;
      }
    }

    publish_cloud->end_index_in_pacp = cur_packet_index_;
    publish_cloud->offset_in_pcap = cur_packet_offset_;
    // measurement timstamp
    if (!publish_cloud->points.empty()) {
      publish_cloud->timestamp = publish_cloud->points[0].timestamp;
    }
    // detect number
    publish_cloud->dectect_number = point_cloud_buffer_.size();
    // callback
    point_cloud_callback_(publish_cloud, index_f_single_thread_++);
    // reset
    ResetDriver();
  }

  return true;
}

bool XRadar2DDriver::ParsePacket(const uint8_t* data, size_t len) {
  // check header
  auto header = reinterpret_cast<const PacketHeader*>(data);
  if (header->sop[0] != 0xa5 || header->sop[1] != 0xa5) {
    std::stringstream ss;
    ss << "Packet Header Error: " << std::hex << (0xFF & header->sop[0])
       << (0xFF & header->sop[1]) << std::dec;
    LOG(INFO) << ss.str();
    return false;
  }
  if (header->major_version == 0x00 && header->minor_version == 0x00) {
    return ParsePacket0_0(data, len);
  } else if (header->major_version == 0x01 && header->minor_version == 0x00) {
    return ParsePacket1_0(data, len);
  } else {
    LOG(INFO) << "Invalid packet, packet version: " +
                     std::to_string(header->major_version) + "." +
                     std::to_string(header->minor_version) + ". new firmware?";
  }
  return false;
}

bool XRadar2DDriver::ParsePacket0_0(const uint8_t* data, size_t len) {
  auto udp_pkt = reinterpret_cast<const RadarPacket*>(data);
  if (len != udp_pkt->header.packet_len) {
    LOG(INFO) << "len is wrong. udp len: " + std::to_string(len) +
                     ". packet len: " +
                     std::to_string(udp_pkt->header.packet_len);
    return false;
  }
  auto point_cloud = point_cloud_buffer_[0];
  if (point_cloud->points.empty()) {
    point_cloud->begin_index_in_pacp = cur_packet_index_;
  }
  // parse ptp timestamp
  double ptp_timestamp_s = ComputeTimeStamp(udp_pkt->timestamp);
  int point_num = (udp_pkt->header.packet_len - 38) / 6;
  auto rs = udp_pkt->resolution;
  // for each block
  for (int blockid = 0; blockid < point_num; blockid++) {
    const auto& block = udp_pkt->data[blockid];
    float dist = block.range * rs.range_resolution;
    float doppler = block.doppler * rs.doppler_resolution;
    float azimuth = block.azimuth * rs.azimuth_resolution;

    // normalize azimuth
    azimuth = normalize_angle_deg(azimuth);
    if (!CheckValid(azimuth, dist)) {
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
  point_cloud->major_version = udp_pkt->header.major_version;
  point_cloud->minor_version = udp_pkt->header.minor_version;
  point_cloud->timestamp = ptp_timestamp_s;
  point_cloud->udp_sequences.push_back(udp_pkt->udp_sequence);
  point_cloud->range_resolution[0] = rs.range_resolution;
  point_cloud->azimuth_resolution[0] = rs.range_resolution;

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

bool XRadar2DDriver::CheckValid(float azimuth, float distance) {
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

bool XRadar2DDriver::ParsePacket1_0(const uint8_t* data, size_t len) {
  auto udp_pkt = reinterpret_cast<const RadarPacket1_0*>(data);
  if (len != udp_pkt->header.packet_len) {
    LOG(INFO) << "len is wrong. udp len: " + std::to_string(len) +
                     ". packet len: " +
                     std::to_string(udp_pkt->header.packet_len);
    return false;
  }
  // detect type
  if (!init_buffer_) {
    InitBufferCloud(udp_pkt->detect_number);
    init_buffer_ = true;
  }

  PointCloudPtr point_cloud = point_cloud_buffer_[udp_pkt->detect_type];
  if (point_cloud->points.empty()) {
    point_cloud->begin_index_in_pacp = cur_packet_index_;
  }
  // parse ptp timestamp
  double ptp_timestamp_s = ComputeTimeStamp(udp_pkt->timestamp);
  int point_num = (udp_pkt->header.packet_len - 38) / 6;
  auto rs = udp_pkt->resolution;
  // for each block
  for (int blockid = 0; blockid < point_num; blockid++) {
    const auto& block = udp_pkt->data[blockid];
    float dist = block.range * rs.range_resolution;
    float doppler = block.doppler * rs.doppler_resolution;
    float azimuth = block.azimuth * rs.azimuth_resolution;

    // normalize azimuth
    azimuth = normalize_angle_deg(azimuth);
    if (!CheckValid(azimuth, dist)) {
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
  point_cloud->major_version = udp_pkt->header.major_version;
  point_cloud->minor_version = udp_pkt->header.minor_version;
  point_cloud->timestamp = ptp_timestamp_s;
  point_cloud->udp_sequences.push_back(udp_pkt->udp_sequence);
  point_cloud->range_resolution[0] = rs.range_resolution;
  point_cloud->azimuth_resolution[0] = rs.azimuth_resolution;
  return true;
}

void XRadar2DDriver::InitBufferCloud(int size) {
  for (int i = point_cloud_buffer_.size(); i < size; i++) {
    auto pc = std::make_shared<PointCloud>();
    ClearPointcloud(pc);
    pc->range_resolution.resize(1);
    pc->azimuth_resolution.resize(1);
    point_cloud_buffer_.push_back(pc);
  }
}

double XRadar2DDriver::ComputeTimeStamp(Timestamp time) {
  uint64_t seconds = time.sec_msb;
  seconds = (seconds << 32) + time.seconds;
  return seconds + time.nano_second * 1e-9;
}
}  // namespace xradar_2d
}  // namespace drivers
}  // namespace autox
