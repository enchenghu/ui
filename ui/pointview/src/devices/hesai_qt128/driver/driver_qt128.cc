
#include "driver_qt128.h"
#include <glog/logging.h>

namespace autox {
namespace drivers {
namespace hesai {

constexpr unsigned int kBlockNumber = 2;
constexpr unsigned int kUnitNum = 128;
constexpr unsigned int kLaserOrderNum = 2;

std::array<double, kRotationMaxUnits> DriverQt128::sin_lookup_table_;
std::array<double, kRotationMaxUnits> DriverQt128::cos_lookup_table_;

static const float kBlockOffsetDual[] = {7.0f, 7.0f};
static const float kBlockOffset[] = {7.0f, 7.0f + 111.11f};

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
              "Currently the driver only work with little endian systems!");

#pragma pack(push, 1)  // force no struct padding

struct Header {
  uint16_t sop;
  uint8_t protocalVersionMajor;
  uint8_t protocalVersionMinor;
  uint8_t reserved[2];

  uint8_t laserNum;
  uint8_t blockNum;
  uint8_t firstBlockReturn;
  uint8_t disUnit;
  uint8_t returnNum;
  uint8_t flags;
};

struct Unit {
  uint16_t distance;
  uint8_t reflectivity;
  uint8_t reserved;
};

struct PandarQt128Block {
  uint16_t azimuth;
  Unit units[kUnitNum];
};

struct Body {
  PandarQt128Block blocks[kBlockNumber];
  uint32_t crc1;
};

struct Tail {
  uint8_t reserved0[5];
  uint8_t modeFlag;
  uint8_t reserved1[6];
  uint8_t returnMode;
  uint16_t motorSpeed;
  int8_t year;
  int8_t month;
  int8_t day;
  int8_t hour;
  int8_t minute;
  int8_t second;

  uint32_t timestamp;
  uint8_t fatoryInformation;
  uint32_t udpSequence;
  uint32_t crc3;
};

struct FunctionSafety {
  uint8_t reserved[17];
};

struct XrtSafety {
  uint8_t reserved[32];
};

struct PandarQt128Packet {
  Header header;
  Body body;
  FunctionSafety functionSafety;
  Tail tail;
  XrtSafety xrtSafety;
};

#pragma pack(pop)

constexpr unsigned int kPacketSize = sizeof(PandarQt128Packet);

static const std::vector<double> kElevAngleMap = {
    -52.506, -50.913, -49.392, -47.961, -46.537, -45.219, -43.903, -42.68,
    -41.457, -40.303, -39.135, -38.038, -36.926, -35.879, -34.813, -33.799,
    -32.766, -31.782, -30.773, -29.82,  -28.846, -27.919, -26.974, -26.065,
    -25.136, -24.244, -23.327, -22.451, -21.56,  -20.7,   -19.813, -18.965,
    -18.099, -17.262, -16.393, -15.566, -14.718, -13.9,   -13.048, -12.238,
    -11.403, -10.594, -9.76,   -8.96,   -8.124,  -7.323,  -6.503,  -5.698,
    -4.885,  -4.092,  -3.265,  -2.476,  -1.654,  -0.868,  -0.045,  0.748,
    1.575,   2.363,   3.18,    3.969,   4.802,   5.591,   6.415,   7.207,
    8.073,   8.873,   9.702,   10.508,  11.346,  12.154,  12.997,  13.813,
    14.663,  15.485,  16.336,  17.167,  18.043,  18.883,  19.748,  20.598,
    21.497,  22.36,   23.247,  24.125,  25.049,  25.943,  26.873,  27.783,
    28.746,  29.677,  30.651,  31.609,  32.629,  33.616,  34.656,  35.676,
    19.265,  20.114,  21.004,  21.864,  22.767,  23.637,  24.539,  25.425,
    26.352,  27.257,  28.216,  29.143,  30.118,  31.066,  32.066,  33.043,
    34.075,  35.086,  36.159,  37.211,  38.328,  39.426,  40.581,  41.732,
    42.96,   44.177,  45.488,  46.82,   48.222,  49.659,  51.266,  52.852};

static const std::vector<double> kAzimuthOffsetMap = {
    7.701,   7.353,  7.11,   6.89,   6.692,  6.528,  6.367,   6.236,   6.104,
    5.985,   5.877,  5.781,  5.697,  5.614,  5.535,  5.462,   5.391,   5.325,
    5.272,   5.222,  5.172,  5.124,  5.076,  5.029,  4.998,   4.96,    4.927,
    4.895,   4.857,  4.825,  4.808,  4.783,  -7.393, -7.36,   -7.309,  -7.275,
    -7.248,  -7.222, -7.194, -7.165, -7.137, -7.12,  -7.104,  -7.077,  -7.059,
    -7.042,  -7.07,  -7.017, -7.048, -7.04,  -7.022, -7.018,  -7.019,  -7.015,
    -7.0,    -7.002, -7.007, -7.01,  -7.022, -7.018, -7.033,  -7.03,   -7.053,
    -7.06,   4.627,  4.629,  4.657,  4.672,  4.689,  4.703,   4.722,   4.738,
    4.76,    4.783,  4.803,  4.828,  4.858,  4.885,  4.915,   4.942,   4.984,
    5.014,   5.043,  5.077,  5.126,  5.167,  5.212,  5.254,   5.307,   5.365,
    5.426,   5.49,   5.538,  5.632,  5.717,  5.804,  -7.34,   -7.38,   -7.426,
    -7.47,   -7.513, -7.559, -7.621, -7.679, -7.737, -7.798,  -7.862,  -7.94,
    -8.022,  -8.104, -8.188, -8.293, -8.404, -8.511, -8.632,  -8.738,  -8.901,
    -9.028,  -9.203, -9.368, -9.551, -9.766, -9.996, -10.257, -10.544, -10.877,
    -11.291, -11.725};

struct LaserOrderItem {
  float offset;
  int channels[kLaserOrderNum];
};

static const std::vector<LaserOrderItem> kLaserOrderTable = std::vector<LaserOrderItem>(
    {{0.600, {99, 65}},   {1.456, {65, 99}},   {2.312, {35, 1}},
     {3.768, {102, 72}},  {4.624, {72, 102}},  {5.48, {38, 8}},
     {6.936, {107, 73}},  {7.792, {73, 107}},  {8.648, {43, 9}},
     {10.104, {110, 80}}, {10.96, {80, 110}},  {11.816, {46, 16}},
     {13.272, {115, 81}}, {14.128, {81, 115}}, {14.984, {51, 17}},
     {16.44, {118, 88}},  {17.296, {88, 118}}, {18.152, {54, 24}},
     {19.608, {123, 89}}, {20.464, {89, 123}}, {21.32, {59, 25}},
     {22.776, {126, 96}}, {23.632, {96, 126}}, {24.488, {62, 32}},
     {25.944, {97, 67}},  {26.8, {67, 97}},    {27.656, {33, 3}},
     {29.112, {104, 70}}, {0.968, {70, 104}},  {30.824, {40, 6}},
     {32.28, {105, 75}},  {33.136, {75, 105}}, {33.992, {41, 11}},
     {35.448, {112, 78}}, {36.304, {78, 112}}, {37.16, {48, 14}},
     {38.616, {113, 83}}, {39.472, {83, 113}}, {40.328, {49, 19}},
     {20.464, {120, 86}}, {42.64, {86, 120}},  {43.496, {56, 22}},
     {44.952, {121, 91}}, {45.808, {91, 121}}, {46.664, {57, 27}},
     {48.12, {128, 94}},  {48.976, {94, 128}}, {49.832, {64, 30}},
     {51.288, {98, 68}},  {52.144, {68, 98}},  {53.000, {34, 4}},
     {54.456, {103, 69}}, {55.312, {69, 103}}, {56.168, {39, 5}},
     {57.624, {106, 76}}, {58.48, {76, 106}},  {59.336, {42, 12}},
     {60.792, {111, 77}}, {61.648, {77, 111}}, {62.504, {47, 13}},
     {63.96, {114, 84}},  {64.816, {84, 114}}, {65.672, {50, 20}},
     {67.128, {119, 85}}, {67.984, {85, 119}}, {68.84, {55, 21}},
     {70.296, {122, 92}}, {71.152, {92, 122}}, {72.008, {58, 28}},
     {73.464, {127, 93}}, {74.32, {93, 127}},  {75.176, {63, 29}},
     {76.632, {100, 66}}, {77.488, {66, 100}}, {78.344, {36, 2}},
     {79.8, {101, 71}},   {80.656, {71, 101}}, {81.512, {37, 7}},
     {82.968, {108, 74}}, {83.824, {74, 108}}, {84.68, {44, 10}},
     {84.68, {109, 79}},  {86.992, {79, 109}}, {87.848, {45, 15}},
     {89.304, {116, 82}}, {90.16, {82, 116}},  {91.016, {52, 18}},
     {92.472, {117, 87}}, {93.328, {87, 117}}, {94.184, {53, 23}},
     {95.64, {124, 90}},  {96.496, {90, 124}}, {97.352, {60, 26}},
     {98.808, {125, 95}}, {99.664, {95, 125}}, {100.52, {61, 3}}});

/* constexpr auto GenerateLaserOffset() {
  std::array<std::array<float, kUnitNum>, kLaserOrderNum> laser_offset{};
  for (const auto& item : kLaserOrderTable) {
    for (unsigned int order = 0; order < kLaserOrderNum; ++order) {
      const auto& ch = item.channels[order] - 1;  // table count from 1
      laser_offset[order][ch] = item.offset;
    }
  }
  return laser_offset;
} */
//constexpr auto kLaserOffset = GenerateLaserOffset();

DriverQt128::DriverQt128() {
  int kLaserNum = kElevAngleMap.size();
  elev_angle_map_.resize(kLaserNum);
  horizatal_azimuth_offset_map_.resize(kLaserNum);
  elev_angle_map_ = std::vector(kElevAngleMap.begin(), kElevAngleMap.end());
  horizatal_azimuth_offset_map_ =
      std::vector(kAzimuthOffsetMap.begin(), kAzimuthOffsetMap.end());
  //
  for (uint16_t rotIndex = 0; rotIndex < cos_lookup_table_.size(); ++rotIndex) {
    const double rotation = kDegreeToRadian * (static_cast<double>(rotIndex) /
                                               kSinCosTableResolution);
    cos_lookup_table_[rotIndex] = cos(rotation);
    sin_lookup_table_[rotIndex] = sin(rotation);
  }
  // init point cloud buffer
  for (size_t i = 0; i < max_point_cloud_buffer_size_; i++) {
    auto point_cloud = std::make_shared<PointCloud>();
    point_cloud->points.reserve(720000);
    point_cloud->udp_scan_sequence.reserve(4000);
    point_cloud_buffer_.push_back(point_cloud);
  }
  resetDriver();
}

void DriverQt128::resetDriver() {
  cur_packet_index_ = 0;
  point_cloud_ = point_cloud_buffer_[point_cloud_buffer_index_];
  point_cloud_->points.clear();
  point_cloud_->udp_scan_sequence.clear();
  point_cloud_->udp_packet_number = 0;
  point_cloud_buffer_index_ =
      (point_cloud_buffer_index_ + 1) % point_cloud_buffer_.size();
}

DriverQt128::~DriverQt128() {
  // Stop threads in case ParseLidarPacket() called
  // after DriverQt128 destructed
  // Stop();
}

bool DriverQt128::ParseLidarPacket(const uint8_t* data, size_t len) {
  if (len != kPacketSize) {
    LOG(INFO) << "len is " << len << ", kPacketSize is " << kPacketSize;
    return false;
  }
  if (!IsValidQt128Packet(data)) {
    return false;
  }
  auto qt128_pkt = reinterpret_cast<const PandarQt128Packet*>(data);
  ParsePacket(qt128_pkt);
  return true;
}

bool DriverQt128::IsValidQt128Packet(const uint8_t* data) {
  if (!data) {
    return false;
  }
  auto qt128_pkt = reinterpret_cast<const PandarQt128Packet*>(data);
  if (qt128_pkt->header.sop != 0xFFEE) {
    LOG(ERROR) << "Start of Packet error: " << std::hex << qt128_pkt->header.sop;
    return false;
  }
  if (qt128_pkt->header.protocalVersionMajor != 0x03 ||
      qt128_pkt->header.protocalVersionMinor != 0x02) {
    LOG(ERROR) << "Lidar UDP packet format version: "
           << qt128_pkt->header.protocalVersionMajor << '.'
           << qt128_pkt->header.protocalVersionMinor << ". New firmware?";
    return false;
  }
  return true;
}

bool DriverQt128::CheckAngle(const int cur_azimuth) {
  int azimuth_gap = 0;
  int kAzimuthThre = 200;
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

void DriverQt128::ParsePacket(const PandarQt128Packet* qt128_pkt) {
  is_dual_return_ = (qt128_pkt->header.returnNum == '\x02');
  struct tm tTm;
  // year since 1900
  tTm.tm_year = qt128_pkt->tail.year;
  // UTC's month start from 1, but mktime/timegm only accept month from 0.
  tTm.tm_mon = qt128_pkt->tail.month - 1;
  tTm.tm_mday = qt128_pkt->tail.day;
  tTm.tm_hour = qt128_pkt->tail.hour;
  tTm.tm_min = qt128_pkt->tail.minute;
  tTm.tm_sec = qt128_pkt->tail.second;
  tTm.tm_isdst = 0;
  auto pkt_timestamp = static_cast<double>(timegm(&tTm));
  // DO
  // The sub second part of the timestamp
  const unsigned int timestamp_in_us = qt128_pkt->tail.timestamp;
  pkt_timestamp += timestamp_in_us * 1e-6;
  const double distance_unit = qt128_pkt->header.disUnit / 1000.0;

  if (point_cloud_->points.empty()) {
    point_cloud_->begin_index_in_pacp = cur_packet_index_;
  }
  point_cloud_->udp_packet_number++;
  for (int i = 0; i < qt128_pkt->header.blockNum; ++i) {
    if (is_dual_return_ && i % 2 == 1) {
      // The second return will be parsed together with the first return,
      // In the dual return mode, We assume that the block number is always even
      // as we have observed in all the Hesai products so far.
      continue;
    }
    const int cur_azimuth = qt128_pkt->body.blocks[i].azimuth;
    CalcPointXYZIT(qt128_pkt, i, qt128_pkt->header.laserNum,
                   static_cast<uint64_t>(pkt_timestamp * 1e9), distance_unit,
                   cur_azimuth / 100.0, qt128_pkt->tail.modeFlag);
    if (CheckAngle(cur_azimuth) && point_cloud_->points.size() > 0 &&
        point_cloud_callback_) {
      point_cloud_->end_index_in_pacp = cur_packet_index_;
      point_cloud_->timestamp = point_cloud_->points[0].timestamp / 1e9;
      point_cloud_->return_mode = qt128_pkt->tail.returnMode;
      point_cloud_callback_(point_cloud_, index_f_single_thread_++);
      // clear
      resetDriver();
    }
    last_azimuth_ = cur_azimuth;
  }
}

float DriverQt128::CosLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return cos_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

float DriverQt128::SinLookupDegree(float angle_in_deg) {
  if (angle_in_deg < 0) {
    angle_in_deg += 360;
  } else if (angle_in_deg >= 360) {
    angle_in_deg -= 360;
  }
  return sin_lookup_table_[angle_in_deg * kSinCosTableResolution];
}

void DriverQt128::CalcPointXYZIT(
    const PandarQt128Packet* qt128_pkt, const unsigned int blockid,
    const unsigned int laser_num, const uint64_t pkt_timestamp,
    const double distance_unit, const double azimuth, const uint8_t mode_flag) {
  // for all the units/points in a block
  std::vector<float> distance;
  std::vector<uint8_t> intensity;
  distance.reserve(2);
  intensity.reserve(2);
  const auto& block = qt128_pkt->body.blocks[blockid];

  uint32_t scans_per_udp = 0;
  uint64_t scan_sequence = 0;
  if (is_dual_return_) {
    scans_per_udp = qt128_pkt->header.blockNum >> 1;
    scan_sequence =
        static_cast<uint64_t>(qt128_pkt->tail.udpSequence) * scans_per_udp + blockid / 2;
  } else {
    scans_per_udp = qt128_pkt->header.blockNum;
    scan_sequence =
        static_cast<uint64_t>(qt128_pkt->tail.udpSequence) * scans_per_udp + blockid;
  }  
  point_cloud_->udp_scan_sequence.push_back(scan_sequence);

  for (unsigned int i = 0; i < laser_num; ++i) {
    const auto& unit = block.units[i];
    distance.clear();
    intensity.clear();
    distance.push_back(unit.distance * distance_unit);
    intensity.push_back(unit.reflectivity & 0xff);
    if (is_dual_return_) {
      const auto& second_unit = qt128_pkt->body.blocks[blockid + 1].units[i];
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
      uint64_t timestamp = pkt_timestamp;
/*       if (is_dual_return_) {
        int order = mode_flag == 0 ? 0 : 1;
        timestamp -= (static_cast<uint64_t>(kBlockOffsetDual[blockid] +
                                            kLaserOffset[order][i]) *
                      1000);
      } else {
        timestamp -= (static_cast<uint64_t>(kBlockOffset[blockid] +
                                            kLaserOffset[blockid][i]) *
                      1000);
      } */
      Point p;
      p.timestamp = timestamp;
      p.laser_id = i;
      p.return_id = (distance.size() == 1) ? 0 : j + 1;
      p.encoder = azimuth;
      p.azimuth = azimuth;
      p.elevation = elev_angle_map_[i];
      p.y = xy_dist *
          CosLookupDegree(horizatal_azimuth_offset_map_[i] + azimuth - 90);
      p.x = xy_dist *
          SinLookupDegree(horizatal_azimuth_offset_map_[i] + azimuth - 90);
      p.z = dist_in_m * SinLookupDegree(elev_angle_map_[i]);
      p.distance = dist_in_m;
      p.intensity = unit.reflectivity & 0xff;
      point_cloud_->points.push_back(p);
    }
  }
}

bool DriverQt128::setIntrinsics(const std::vector<double>& elevation,
                                    const std::vector<double>& azimuth,
                                    const std::vector<double>& distance) {
  elev_angle_map_ = elevation;
  horizatal_azimuth_offset_map_ = azimuth;
  distance_offset_map_ = distance;
  return true;
}

bool DriverQt128::getIntrinsics(std::vector<double>& elevation,
                                    std::vector<double>& azimuth,
                                    std::vector<double>& distance) {
  elevation = elev_angle_map_;
  azimuth = horizatal_azimuth_offset_map_;
  distance = distance_offset_map_;
  return true;
}

}  // namespace autox::drivers::hesai
}
}
