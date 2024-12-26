/******************************************************************************
 * Copyright 2022 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "xrt/record/record_reader.h"

#include <utility>

namespace autox {
namespace recorder {

using autox::xrt::proto::SectionType;

RecordReader::~RecordReader() {}

RecordReader::RecordReader(const std::string& file) {
  file_reader_.reset(new RecordFileReader());
  if (!file_reader_->Open(file)) {
    LOG(ERROR) << "Failed to open record file: " << file;
    switch (file_reader_->GetError()) {
      case RecordFileReader::E_NOT_EXIST:
        last_error_ = E_NOT_EXIST;
        break;
      case RecordFileReader::E_IO_OPEN:
      case RecordFileReader::E_IO_READ:
        last_error_ = E_IO;
        break;
      case RecordFileReader::E_FILE_BROKEN:
        last_error_ = E_FILE_BROKEN;
        break;
      default:
        break;
    }
    return;
  }
  chunk_.reset(new ChunkBody());
  is_valid_ = true;
  header_ = file_reader_->GetHeader();
  if (file_reader_->ReadIndex()) {
    index_ = file_reader_->GetIndex();
  } else {
    LOG(ERROR) << "ReadIndex fail." << file << " maybe is broken";
    RecoverIndex();
  }
  for (int i = 0; i < index_.indexes_size(); ++i) {
    auto single_idx = index_.mutable_indexes(i);
    if (single_idx->type() != SectionType::SECTION_CHANNEL) {
      continue;
    }
    if (!single_idx->has_channel_cache()) {
      LOG(ERROR) << "Single channel index does not have channel_cache.";
      continue;
    }
    auto channel_cache = single_idx->mutable_channel_cache();
    channel_info_.insert(std::make_pair(channel_cache->name(), *channel_cache));
  }
  record_debug_info_ = file_reader_->GetRecordDebugInfo();

  file_reader_->Reset();
}

void RecordReader::Reset() {
  file_reader_->Reset();
  reach_end_ = false;
  message_index_ = 0;
  chunk_.reset(new ChunkBody());
}

std::set<std::string> RecordReader::GetChannelList() const {
  std::set<std::string> channel_list;
  for (auto& item : channel_info_) {
    channel_list.insert(item.first);
  }
  return channel_list;
}

autox::xrt::proto::BagFileDebugInfo RecordReader::GetDebugInfo() {
  autox::xrt::proto::BagFileDebugInfo bag_file_debug_info;
  bag_file_debug_info.mutable_index()->CopyFrom(index_);
  bag_file_debug_info.mutable_record_debug_info()->CopyFrom(record_debug_info_);
  return bag_file_debug_info;
}

bool RecordReader::ReadMessage(autox::xrt::record::RecordMessage* message,
                               uint64_t begin_time, uint64_t end_time) {
  if (!is_valid_) {
    return false;
  }

  if (begin_time > header_.end_time() || end_time < header_.begin_time()) {
    last_error_ = E_TIME_OUT_OF_HEADER;
    return false;
  }

  while (message_index_ < chunk_->messages_size()) {
    const auto& next_message = chunk_->messages(message_index_);
    uint64_t time = next_message.time();
    if (time > end_time) {
      last_error_ = E_END_TIME_REACHED;
      return false;
    }
    ++message_index_;
    if (time < begin_time) {
      continue;
    }

    message->channel_name = next_message.channel_name();
    message->content = next_message.content();
    message->time = time;
    return true;
  }

  LOG(INFO) << "Read next chunk.";
  if (ReadNextChunk(begin_time, end_time)) {
    LOG(INFO) << "Read chunk successfully.";
    message_index_ = 0;
    return ReadMessage(message, begin_time, end_time);
  }
  last_error_ = file_reader_->GetError() == RecordFileReader::E_IO_READ
                    ? E_IO
                    : E_NO_CHUNK_TO_READ;
  LOG(INFO) << "No chunk to read.";
  return false;
}

bool RecordReader::ReadMessage(ErrorEnum* error,
                               autox::xrt::record::RecordMessage* message,
                               uint64_t begin_time, uint64_t end_time) {
  if (ReadMessage(message, begin_time, end_time)) {
    *error = NONE;
    return true;
  }
  *error = last_error_;
  return false;
}

bool RecordReader::ReadNextChunk(uint64_t begin_time, uint64_t end_time) {
  bool skip_next_chunk_body = false;
  while (!reach_end_) {
    Section section;
    if (!file_reader_->ReadSection(&section)) {
      if (file_reader_->EndOfFile()) {
        reach_end_ = true;
      } else {
        LOG(ERROR) << "Failed to read section, file: "
                   << file_reader_->GetPath();
      }
      return false;
    }
    switch (section.type) {
      case SectionType::SECTION_INDEX: {
        file_reader_->SkipSection(section.size);
        reach_end_ = true;
        break;
      }
      case SectionType::SECTION_DEBUG_INFO: {
        file_reader_->SkipSection(section.size);
        break;
      }
      case SectionType::SECTION_CHANNEL: {
        LOG(INFO) << "Read channel section of size: " << section.size;
        Channel channel;
        if (!file_reader_->ReadSection<Channel>(section.size, &channel)) {
          LOG(ERROR) << "Failed to read channel section.";
          return false;
        }
        break;
      }
      case SectionType::SECTION_CHUNK_HEADER: {
        LOG(INFO) << "Read chunk header section of size: " << section.size;
        ChunkHeader header;
        if (!file_reader_->ReadSection<ChunkHeader>(section.size, &header)) {
          LOG(ERROR) << "Failed to read chunk header section.";
          return false;
        }
        if (header.end_time() < begin_time) {
          skip_next_chunk_body = true;
        }
        if (header.begin_time() > end_time) {
          return false;
        }
        break;
      }
      case SectionType::SECTION_CHUNK_BODY: {
        if (skip_next_chunk_body) {
          file_reader_->SkipSection(section.size);
          skip_next_chunk_body = false;
          break;
        }

        chunk_.reset(new ChunkBody());
        if (!file_reader_->ReadSection<ChunkBody>(section.size, chunk_.get())) {
          LOG(ERROR) << "Failed to read chunk body section.";
          return false;
        }
        return true;
      }
      default: {
        LOG(ERROR) << "Invalid section, type: " << section.type
                   << ", size: " << section.size;
        return false;
      }
    }
  }
  return false;
}

uint64_t RecordReader::GetMessageNumber(const std::string& channel_name) const {
  auto search = channel_info_.find(channel_name);
  if (search == channel_info_.end()) {
    return 0;
  }
  return search->second.message_number();
}

const std::string& RecordReader::GetMessageType(
    const std::string& channel_name) const {
  auto search = channel_info_.find(channel_name);
  if (search == channel_info_.end()) {
    return kEmptyString;
  }
  return search->second.message_type();
}

const std::string& RecordReader::GetProtoDesc(
    const std::string& channel_name) const {
  auto search = channel_info_.find(channel_name);
  if (search == channel_info_.end()) {
    return kEmptyString;
  }
  return search->second.proto_desc();
}

void RecordReader::RecoverIndex() {
  std::unordered_map<std::string, ChannelInfo> channel_info_map;

  file_reader_->Reset();
  while (!file_reader_->EndOfFile()) {
    Section section;
    if (!file_reader_->ReadSection(&section)) {
      LOG(ERROR) << "read section failed, try next.";
      continue;
    }
    if (section.type == SectionType::SECTION_INDEX) {
      break;
    }
    if (section.type == SectionType::SECTION_DEBUG_INFO) {
      break;
    }
    switch (section.type) {
      case SectionType::SECTION_CHANNEL: {
        Channel chan;
        if (!file_reader_->ReadSection<Channel>(section.size, &chan)) {
          LOG(INFO) << "one channel section broken, skip it.";
          break;
        }
        SingleIndex* single_index = index_.add_indexes();
        single_index->set_type(SectionType::SECTION_CHANNEL);
        // single_index->set_position(pos);
        ChannelCache* channel_cache = new ChannelCache();
        channel_cache->set_name(chan.name());
        channel_cache->set_message_number(0);
        channel_cache->set_message_type(chan.message_type());
        channel_cache->set_proto_desc(chan.proto_desc());
        single_index->set_allocated_channel_cache(channel_cache);
        auto it = channel_info_map.find(chan.name());
        if (it == channel_info_map.end()) {
          ChannelInfo channel_info;
          channel_info.set_channel_name(chan.name());
          channel_info_map.insert(std::make_pair(chan.name(), channel_info));
        }
        break;
      }
      case SectionType::SECTION_CHUNK_BODY: {
        ChunkBody cbd;
        if (!file_reader_->ReadSection<ChunkBody>(section.size, &cbd)) {
          LOG(INFO) << "one chunk body section broken, skip it";
          break;
        }
        for (int idx = 0; idx < cbd.messages_size(); ++idx) {
          auto& message = cbd.messages(idx);
          auto it = channel_info_map.find(message.channel_name());
          if (it != channel_info_map.end()) {
            it->second.set_message_number(it->second.message_number() + 1);
          }
        }
        break;
      }
      case SectionType::SECTION_CHUNK_HEADER: {
        ChunkHeader chdr;
        if (!file_reader_->ReadSection<ChunkHeader>(section.size, &chdr)) {
          LOG(INFO) << "one chunk header section broken, skip it.";
        }
        break;
      }
      default: {
        LOG(ERROR) << "this section should not be here, section type: "
                   << section.type;
      }
    }  // end for switch
  }
  for (int i = 0; i < index_.indexes_size(); i++) {
    SingleIndex* single_index = index_.mutable_indexes(i);
    if (single_index->type() == SectionType::SECTION_CHANNEL) {
      ChannelCache* channel_cache = single_index->mutable_channel_cache();
      if (channel_info_map.find(channel_cache->name()) !=
          channel_info_map.end()) {
        channel_cache->set_message_number(
            channel_info_map[channel_cache->name()].message_number());
      }
    }
  }
}

}  // namespace recorder
}  // namespace autox
