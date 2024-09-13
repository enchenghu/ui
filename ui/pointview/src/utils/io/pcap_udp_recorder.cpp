#include "pcap_udp_recorder.h"

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap.h>
#include <string.h>
#include <QProgressDialog>

#include <net/ethernet.h>

#define PCAP_UDP_PACKET_HAEADER_LENGTH 42
#define ETHERNET_MTU (1500)

namespace autox {
namespace pointview {

PcapUdpRecorder::PcapUdpRecorder(std::shared_ptr<DeviceContext> device_context) {
  device_context_ = device_context;
  pcap_udp_header_.resize(PCAP_UDP_PACKET_HAEADER_LENGTH);
  std::fill(pcap_udp_header_.begin(), pcap_udp_header_.end(), 0);
  // set ethernet header
  uint8_t mac[6] = {0xb0, 0x7b, 0x25, 0x3e, 0x28, 0xfd};
  auto eth_ptr = reinterpret_cast<ether_header*>(pcap_udp_header_.data());
  memcpy(eth_ptr->ether_shost, mac, 6);
  memcpy(eth_ptr->ether_dhost, mac, 6);
  eth_ptr->ether_type = htons(ETHERTYPE_IP);
  // set ip header
  auto ip_ptr = reinterpret_cast<struct ip*>(pcap_udp_header_.data() + 14);
  ip_ptr->ip_v = 4;
  ip_ptr->ip_hl = 5;
  ip_ptr->ip_id = 0;
  ip_ptr->ip_off = htons(IP_DF);
  ip_ptr->ip_ttl = 10;
  ip_ptr->ip_p = IPPROTO_UDP;
  ip_ptr->ip_dst.s_addr = inet_addr("127.0.0.1");
  ip_ptr->ip_src.s_addr = inet_addr("127.0.0.1");
  // set udp header
  auto udphdr_ptr = reinterpret_cast<udphdr*>(pcap_udp_header_.data() + 34);
  udphdr_ptr->source = htons(12344);
  udphdr_ptr->dest = htons(12345);
  resetBufferQueue();
}

void PcapUdpRecorder::resetBufferQueue() 
{
  recorder_queue_free.clear();
  recorder_queue_done.clear();
  for(int i  = 0; i < 20000 * 10 * 15; i++){
    recorder_queue_free.put(std::vector<uint8_t>());
  }
  wait_record_ = false;
}
void PcapUdpRecorder::setRecordStatus(bool is_recording) {
  is_recording_ = is_recording;
}

void PcapUdpRecorder::stopRecordTask() 
{
  //is_recording_ = false;
  if(save_thread_){
    using namespace std::chrono_literals;
    QProgressDialog progress("saving record file, in progress...", "Abort", 0, 100, nullptr);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    progress.setValue(0);
    while(1){
      if (progress.wasCanceled()) {
        device_context_->updateRecordFi(true);
        break;
      }
      auto state = device_context_->getPlayerState();
      double rate = state.rate_record;
      progress.setValue((int)(rate * 100));
      if(state.record_fi) break;
      std::this_thread::sleep_for(50ms);
    } 
    progress.setValue(100);
    device_context_->updateRecordFi(false);
    device_context_->updateRecordRate(0.0);
    if (save_thread_->joinable()) {
      save_thread_->join();
    }
  }
  save_thread_ = nullptr;
}
void PcapUdpRecorder::startRecordTask() 
{
  save_thread_ = std::make_unique<std::thread>([this]() {
    // open file
    if(!device_context_) return;
    std::string file = device_context_->getPlayerState().record_path;
    if(file.empty()) {
      std::cout << "error!!!! record file path is not set!!!!!" << std::endl;
      return;
    }
  	time_t rawtime;
    struct tm *ptminfo;
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
    ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
    ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
    std::string time_str = 	std::to_string(ptminfo->tm_year + 1900) + \
      "-" + std::to_string(ptminfo->tm_mon + 1) + \
      "-" + std::to_string(ptminfo->tm_mday) + \
      "-" + std::to_string(ptminfo->tm_hour) + \
      "-" + std::to_string(ptminfo->tm_min) + \
      "-" + std::to_string(ptminfo->tm_sec) ;

    file += "/pcap_data_"  + time_str + ".pcap";
    std::cout << "==========file name is =======" << file << std::endl;
    pcap_t* p = NULL;
    struct pcap_pkthdr h;
    p = pcap_open_dead(DLT_EN10MB, 0x0000ffff);
    if (NULL == p) {
      fprintf(stderr, "pcap_open_dead failed. ");
      return;
    }
    fp_ = pcap_dump_open(p, file.c_str());
    if (NULL == fp_) {
      fprintf(stderr, "pcap_dump_open failed. ");
      return;
    }
    uint8_t buffer[ETHERNET_MTU + PCAP_UDP_PACKET_HAEADER_LENGTH];
    memcpy(buffer, pcap_udp_header_.data(), PCAP_UDP_PACKET_HAEADER_LENGTH);
    using namespace std::chrono_literals;
    int size_left_total = 0;
    bool left_inited = false;
    while (is_recording_ || !recorder_queue_done.empty()) {
      if(device_context_->getPlayerState().record_fi) break;
      if(!is_recording_ && !left_inited){
        size_left_total = recorder_queue_done.size();
        left_inited = true;
      }
      if(!is_recording_){
        double rate_temp = 1 - (double)recorder_queue_done.size() / (double)size_left_total;
        device_context_->updateRecordRate(rate_temp);
      }
      std::vector<uint8_t> data_v;
      if (recorder_queue_done.get(data_v)) {
        std::cout << "warnning!! recorder_queue_done is empty, get timeout" << std::endl;
        break;
      }
      auto data_size = data_v.size();
      if (data_size > ETHERNET_MTU) {
        // drop this packet
        recorder_queue_free.put(data_v);
        continue;
      }
      auto ip_ptr = reinterpret_cast<struct ip*>(buffer + 14);
      auto udphdr_ptr = reinterpret_cast<udphdr*>(buffer + 34);
      udphdr_ptr->len = htons(data_size + 8);
      udphdr_ptr->dest = htons(record_port_);
      ip_ptr->ip_len = htons(data_size + 28);
      memcpy(buffer + PCAP_UDP_PACKET_HAEADER_LENGTH, data_v.data(),
            data_size);
      recorder_queue_free.put(data_v);
      gettimeofday(&h.ts, NULL);
      h.caplen = PCAP_UDP_PACKET_HAEADER_LENGTH + data_size;
      h.len = h.caplen;
      pcap_dump((uint8_t*)fp_, &h, buffer);
    }
    device_context_->updateRecordFi(true);
    pcap_dump_close(fp_);
    resetBufferQueue();
    std::cout << "==========quit Record Task=======" << std::endl;
  });
}

void PcapUdpRecorder::setMaxRecordPacket(size_t n) {
  max_record_udp_packet_number_ = n;
}

size_t PcapUdpRecorder::getByteCount() { return byte_cnt_; }

size_t PcapUdpRecorder::getPacketCount() { return recorder_buffer_.size(); }

void PcapUdpRecorder::addPacketData(const std::vector<uint8_t>& data) {
  if (is_recording_ &&
      recorder_buffer_.size() < max_record_udp_packet_number_) {
    recorder_buffer_.push_back(data);
    byte_cnt_ += data.size();
  }
}

void PcapUdpRecorder::addPacketData(const uint8_t* data, size_t len, int port) {
  if(is_recording_) {
    record_port_ = port;
    std::vector<uint8_t> data_v;
    if(wait_record_){
      if(recorder_queue_free.size() > 20000 * 10 * 10) {
        std::cout << "==========start Record Task again=======" << std::endl;
        wait_record_ = false;
        device_context_->updateRecordWait(wait_record_);
        recorder_queue_free.get(data_v);
        data_v = std::vector<uint8_t>(data, data + len);
        recorder_queue_done.put(data_v);
        byte_cnt_ += len;
      }
    } else {
      if(recorder_queue_free.empty()){
        wait_record_ = true;
        device_context_->updateRecordWait(wait_record_);
        std::cout << "==========wait Record Task=======" << std::endl;
      } else {
        recorder_queue_free.get(data_v);
        data_v = std::vector<uint8_t>(data, data + len);
        recorder_queue_done.put(data_v);
        byte_cnt_ += len;
      }
    }
  }
}

void PcapUdpRecorder::clear() {
  recorder_buffer_.clear();
  byte_cnt_ = 0;
}

bool PcapUdpRecorder::save(const std::string& file,
                           std::function<bool(double)> progress_cb) {
  if (recorder_buffer_.size() == 0) {
    return false;
  }
  // open file
  pcap_t* p = NULL;
  pcap_dumper_t* fp = NULL;
  struct pcap_pkthdr h;
  p = pcap_open_dead(DLT_EN10MB, 0x0000ffff);
  if (NULL == p) {
    fprintf(stderr, "pcap_open_dead failed. ");
    return false;
  }
  fp = pcap_dump_open(p, file.c_str());
  if (NULL == fp) {
    fprintf(stderr, "pcap_dump_open failed. ");
    return false;
  }
  uint8_t buffer[ETHERNET_MTU];
  memcpy(buffer, pcap_udp_header_.data(), PCAP_UDP_PACKET_HAEADER_LENGTH);
  for (size_t i = 0; i < recorder_buffer_.size(); i++) {
    auto data_size = recorder_buffer_[i].size();
    if (data_size > ETHERNET_MTU - PCAP_UDP_PACKET_HAEADER_LENGTH) {
      // drop this packet
      continue;
    }
    auto ip_ptr = reinterpret_cast<struct ip*>(buffer + 14);
    auto udphdr_ptr = reinterpret_cast<udphdr*>(buffer + 34);
    udphdr_ptr->len = htons(data_size + 8);
    ip_ptr->ip_len = htons(data_size + 28);
    memcpy(buffer + PCAP_UDP_PACKET_HAEADER_LENGTH, recorder_buffer_[i].data(),
           data_size);
    gettimeofday(&h.ts, NULL);
    h.caplen = PCAP_UDP_PACKET_HAEADER_LENGTH + data_size;
    h.len = h.caplen;
    pcap_dump((uint8_t*)fp, &h, buffer);
    if (progress_cb) {
      double v = 1.0 * (i + 1) / recorder_buffer_.size();
      if (!progress_cb(v)) {
        // cancel
        pcap_dump_close(fp);
        return false;
      }
    }
  }
  pcap_dump_close(fp);
  return true;
}

}  // namespace pointview
}  // namespace autox