/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef PCAP_UDP_PARSER_H
#define PCAP_UDP_PARSER_H

#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <pcap/pcap.h>

#include "utils/common/device_context.h"

namespace autox {
namespace pointview {
// only support UDP packet size < 1500
class PcapUdpParser {
  using PacketCallbackType = std::function<void(const uint8_t*, size_t)>;
  using PacketIndexCallbackType = std::function<void(size_t)>;
  using PacketOffsetCallbackType = std::function<void(long int)>;
  using ResetDriverCallbackType = std::function<void(void)>;

 public:
  PcapUdpParser(std::shared_ptr<autox::pointview::DeviceContext> device_context)
      : device_context_(device_context){
    file_offset_maps_.clear();
    file_offset_r_maps_.clear();
    pcap_index_maps_.clear();
    pcap_cnt_map_.clear();
  };
  ~PcapUdpParser();
  void setResetDriverCallback(ResetDriverCallbackType cb) { reset_driver_cb_ = cb; }
  void setUdpCallback(PacketCallbackType cb) { udp_cb_ = cb; }
  void setPacketIndexCallback(PacketIndexCallbackType cb) { packet_index_cb_ = cb; }
  void setPacketOffsetCallback(PacketOffsetCallbackType cb) { packet_offset_cb_ = cb; }

  void setFilter(std::string ip, uint16_t port);
  void setTargetUdpLength(uint32_t len) { target_udp_len_ = len; };
  std::string getPcapFilePath() { return pcap_file_; };
  size_t getPacketNumber();
  // open and close
  bool open(const std::string& file);
  void close();
  bool isOpen();
  // get packet
  bool getPacket(uint8_t* data, size_t* len, double* timestamp = nullptr,
                 int* port = nullptr);
  bool eof() { return eof_; };
  bool readFileOffset(std::string);
  bool readPcapIndex(std::string);
  bool readPcapMemory(std::string folder);
  void reset();
  void breakParseAllPackets(bool);
  bool parseAllPackets(std::function<bool(double)> progress_cb = nullptr);
  bool getParserState() { return ifInited; }
  int getFrameNum() {return file_offset_maps_[pcap_file_real_].size() - 1;}
  void setPcapSearchMap(int index, long int p){
    pcap_offset_map_[index] = p;
    offset_index_map_[p] = index;
  }
  void setPcapSearchMap(int index){
    pcap_offset_map_[index] = ftell(pcap_file(pcap_handle_));
  }
  void setPcapBeginIndexMap(int index, size_t p){pcap_index_map_[index] = p;}
  int findFrameIndexInMap(long int offset){
    if(file_offset_r_maps_[pcap_file_real_].find(offset) != file_offset_r_maps_[pcap_file_real_].end())
      return file_offset_r_maps_[pcap_file_real_][offset];
    return -1;
  }
  //void buildFileOffsetMap(std::string file_name) {file_offset_maps_[file_name] =  std::map<int, long>();}
  void setFileOffsetMap(int index, long offset) {
    if(outOffsetFile_.is_open()) {
      outOffsetFile_ << offset << ',';  
      std::cout << "=====outOffsetFile_ write, index is " << index  << ", offset is " << offset<< std::endl;
    }; 
    file_offset_maps_[pcap_file_real_][index] = offset;
    file_offset_r_maps_[pcap_file_real_][offset] = index;
  }

  void setFileOffsetMap(int index) {
    long offset = ftell(pcap_file(pcap_handle_));
    file_offset_maps_[pcap_file_real_][index] = offset;
    file_offset_r_maps_[pcap_file_real_][offset] = index;
    pcap_index_maps_[pcap_file_real_][index] = index;
  }

  void setPcapIndexMap(int index, size_t p) {
    if(outPcapIndexFile_.is_open()) {
      outPcapIndexFile_ << p << ',';  
    };
    pcap_index_maps_[pcap_file_real_][index] = p;
  }

  void writeFileEnd(){
    if(outPcapIndexFile_.is_open()) {
      outPcapIndexFile_ << std::endl;  
      outPcapIndexFile_.close();
    }
    if(outOffsetFile_.is_open()){
      outOffsetFile_ << std::endl;  
      outOffsetFile_.close();
    }
  }

  int findCntInMap(int);
 private:
  std::string pcap_file_{""};
  std::string pcap_file_real_{""};
  pcap_t* pcap_handle_{nullptr};
  int pcap_hdr_pos_{0};
  bool eof_{true};
  PacketCallbackType udp_cb_;
  PacketIndexCallbackType packet_index_cb_;
  PacketOffsetCallbackType packet_offset_cb_;
  ResetDriverCallbackType reset_driver_cb_;
  // filter
  std::string ip_;
  uint16_t port_;
  uint32_t target_udp_len_{0};
  std::string filter_{""};
  bpf_program pcap_packet_filter_;
  std::ofstream outOffsetFile_;
  std::ofstream outPcapIndexFile_;
  //
  char err_buf_[PCAP_ERRBUF_SIZE];
  size_t cnt_{0};
  bool ifInited{false};
  bool breakloop_{false};
  std::shared_ptr<autox::pointview::DeviceContext> device_context_;
  std::map<int, long> pcap_offset_map_;
  std::map<long, int> offset_index_map_;
  std::map<int, size_t> pcap_index_map_;
  std::map<std::string, std::map<int, long>> file_offset_maps_;
  std::map<std::string, std::map<long, int>> file_offset_r_maps_;
  std::map<std::string, std::map<int, size_t>> pcap_index_maps_;
  std::map<std::string, size_t> pcap_cnt_map_;
  std::string pcap_mem_path;

};

}  // namespace pointview
}  // namespace autox

#endif  // PCAP_UDP_PARSER_H
