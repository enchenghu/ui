/******************************************************************************
 * Copyright 2024 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/20/24.
//

#include "blidar_adc_plot.h"

#include "utils/utils.h"

struct PacketHeader {
  uint8_t sop[2];         // 0xEEFF 2bytes
  uint8_t major_version;  // 0xff
  uint8_t minor_version;  // 0x01
  uint16_t data_size;
  uint16_t first_sample;
  uint16_t row_number;
  uint16_t column_number;
  uint8_t status;
  uint8_t reserve[3];
};

struct AdcPacket {
  PacketHeader header;
  uint8_t data[0];
};

using namespace std::chrono_literals;

namespace autox {
namespace pointview {

BLidarAdcPlot::BLidarAdcPlot(std::shared_ptr<DeviceContext> device_context)
    : AdcPlot(device_context) {
  setParameterTitle("Row", "Column");
  udp_receive_port_ = 2490;
}

BLidarAdcPlot::~BLidarAdcPlot() {}

void BLidarAdcPlot::ParseAdcPacket(const uint8_t* data, size_t len) {
  // check packet
  if (!CheckPacket(data, len)) {
    return;
  }
  if (render_) {
    return;
  }
  // save adc data
  auto udp_pkt = reinterpret_cast<const AdcPacket*>(data);
  if (udp_pkt->header.status != 0) {
    set_status(false, udp_pkt->header.status);
    StopCollectData();
    return;
  }
  // check data valid
  if (udp_pkt->header.row_number != current_parameter_.row) {
    LOG(INFO) << "Row number error. Receive: " << udp_pkt->header.row_number;
    return;
  }
  if (udp_pkt->header.column_number != current_parameter_.column) {
    LOG(INFO) << "Column number error. Receive: "
              << udp_pkt->header.column_number;
    return;
  }
  adc_title_ = "row: " + std::to_string(current_parameter_.row) +
               " , column: " + std::to_string(current_parameter_.column);
  for (int i = 0; i < udp_pkt->header.data_size; i += 2) {
    uint16_t value =
        (static_cast<uint16_t>(udp_pkt->data[i + 1]) << 8) | udp_pkt->data[i];
    bool is_negative = (value & 0x8000) != 0;
    if (is_negative) {
      value = ~value + 1;
    }
    int16_t signed_value = static_cast<int16_t>(value);
    if (is_negative) {
      signed_value = -signed_value;
    }
    adc_data_.push_back(signed_value);
  }
  // recover ui
  if (adc_data_.size() >= need_sample_number_) {
    if (collect_mode_ == CollectMode::Multi) {
      collect_finish_ = true;
    } else {
      render_ = true;
    }
  }
}

bool BLidarAdcPlot::CheckPacket(const uint8_t* data,
                                size_t len) {  // check packet
  if (len <= 18) {
    return false;
  }
  auto udp_pkt = reinterpret_cast<const AdcPacket*>(data);
  // check head
  if (udp_pkt->header.sop[0] != 0xee || udp_pkt->header.sop[1] != 0xff) {
    std::stringstream ss;
    ss << "Invalid adc packet, head need be 0xeeff, Receive: 0x"
       << Uint8toHex(udp_pkt->header.sop[0])
       << Uint8toHex(udp_pkt->header.sop[1]);
    LOG(INFO) << ss.str();
    return false;
  }
  // check tail
  if (data[len - 2] != 0x0d || data[len - 1] != 0x0a) {
    std::stringstream ss;
    ss << "Invalid adc packet, tail need be 0x0d0a, Receive: 0x"
       << Uint8toHex(data[len - 2]) << Uint8toHex(data[len - 1]);
    LOG(INFO) << ss.str();
    return false;
  }
  // check version
  if (udp_pkt->header.major_version != 0x05 ||
      udp_pkt->header.minor_version != 0x10) {
    std::stringstream ss;
    ss << "Invalid log packet, version need be 5.16, Receive: "
       << udp_pkt->header.major_version << '.' << udp_pkt->header.minor_version;
    LOG(INFO) << ss.str();
    return false;
  }
  // check data size
  if (udp_pkt->header.data_size + 18 != len) {
    std::stringstream ss;
    ss << "Invalid log packet, packet size need be data size + 18, "
          "packet size:"
       << len << ", data size:" << udp_pkt->header.data_size;
    LOG(INFO) << ss.str();
    return false;
  }
  return true;
}

void BLidarAdcPlot::set_position(int x, int y) {
  int row = y + 1;
  int column = x + 1;
  if (select_status_) {
    AddPoint(row, column);
  } else {
    if (getFirstParameter() != row) {
      setFirstParameter(row);
    }
    if (getSecondParameter() != column) {
      setSecondParameter(column);
    }
  }
}

void BLidarAdcPlot::SpinboxValueChangeSlot() {
  int y = getFirstParameter() - 1;
  int x = getSecondParameter() - 1;
  emit PostionValueChanged(x, y);
}

std::string BLidarAdcPlot::FillParameter(int f_p, int s_p) {
  std::string ip = getIp();
  current_parameter_.row = f_p - 1;
  current_parameter_.column = s_p - 1;
  std::string url =
      ip + "/adc-info?Gt0x36&row=" + std::to_string(current_parameter_.row) +
      "&column=" + std::to_string(current_parameter_.column) +
      "&sample=" + std::to_string(need_sample_number_) +
      "&udpport=" + std::to_string(udp_receive_port_);

  return url;
}

QString BLidarAdcPlot::GetSavePath(int index) {
  return save_dir_ + "/" + QString::number(index + 1) + "_" +
         QString::number(current_parameter_.row) + "_" +
         QString::number(current_parameter_.column) + ".csv";
}

double BLidarAdcPlot::AdcToVolt(int16_t adc_value) { return (double)adc_value; }

}  // namespace pointview
}  // namespace autox
