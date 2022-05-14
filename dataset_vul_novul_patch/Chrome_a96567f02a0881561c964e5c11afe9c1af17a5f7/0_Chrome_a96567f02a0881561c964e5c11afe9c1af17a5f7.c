QuicErrorCode QuicStreamSequencerBuffer::OnStreamData(
    QuicStreamOffset starting_offset,
    QuicStringPiece data,
    QuicTime timestamp,
    size_t* const bytes_buffered,
    std::string* error_details) {
  CHECK_EQ(destruction_indicator_, 123456) << "This object has been destructed";
  *bytes_buffered = 0;
  QuicStreamOffset offset = starting_offset;
  size_t size = data.size();
  if (size == 0) {
    *error_details = "Received empty stream frame without FIN.";
    return QUIC_EMPTY_STREAM_FRAME_NO_FIN;
  }

  std::list<Gap>::iterator current_gap = gaps_.begin();
  while (current_gap != gaps_.end() && current_gap->end_offset <= offset) {
     ++current_gap;
   }
 
  if (current_gap == gaps_.end()) {
    *error_details = "Received stream data outside of maximum range.";
    return QUIC_INTERNAL_ERROR;
  }
 
  if (offset < current_gap->begin_offset &&
      offset + size <= current_gap->begin_offset) {
    QUIC_DVLOG(1) << "Duplicated data at offset: " << offset
                  << " length: " << size;
    return QUIC_NO_ERROR;
  }
  if (offset < current_gap->begin_offset &&
      offset + size > current_gap->begin_offset) {
    string prefix(data.data(), data.length() < 128 ? data.length() : 128);
    *error_details =
        QuicStrCat("Beginning of received data overlaps with buffered data.\n",
                   "New frame range [", offset, ", ", offset + size,
                   ") with first 128 bytes: ", prefix, "\n",
                   "Currently received frames: ", GapsDebugString(), "\n",
                   "Current gaps: ", ReceivedFramesDebugString());
    return QUIC_OVERLAPPING_STREAM_DATA;
  }
  if (offset + size > current_gap->end_offset) {
    string prefix(data.data(), data.length() < 128 ? data.length() : 128);
    *error_details = QuicStrCat(
        "End of received data overlaps with buffered data.\nNew frame range [",
        offset, ", ", offset + size, ") with first 128 bytes: ", prefix, "\n",
        "Currently received frames: ", ReceivedFramesDebugString(), "\n",
        "Current gaps: ", GapsDebugString());
    return QUIC_OVERLAPPING_STREAM_DATA;
   }
 
  if (offset + size > total_bytes_read_ + max_buffer_capacity_bytes_ ||
      offset + size < offset) {
     *error_details = "Received data beyond available range.";
     return QUIC_INTERNAL_ERROR;
   }

  if (current_gap->begin_offset != starting_offset &&
      current_gap->end_offset != starting_offset + data.length() &&
      gaps_.size() >= kMaxNumGapsAllowed) {
    *error_details = "Too many gaps created for this stream.";
    return QUIC_TOO_MANY_FRAME_GAPS;
  }

  size_t total_written = 0;
  size_t source_remaining = size;
  const char* source = data.data();
  while (source_remaining > 0) {
    const size_t write_block_num = GetBlockIndex(offset);
    const size_t write_block_offset = GetInBlockOffset(offset);
    DCHECK_GT(blocks_count_, write_block_num);

    size_t block_capacity = GetBlockCapacity(write_block_num);
    size_t bytes_avail = block_capacity - write_block_offset;

    if (offset + bytes_avail > total_bytes_read_ + max_buffer_capacity_bytes_) {
      bytes_avail = total_bytes_read_ + max_buffer_capacity_bytes_ - offset;
    }

    if (blocks_ == nullptr) {
      blocks_.reset(new BufferBlock*[blocks_count_]());
      for (size_t i = 0; i < blocks_count_; ++i) {
        blocks_[i] = nullptr;
      }
    }

    if (write_block_num >= blocks_count_) {
      *error_details = QuicStrCat(
          "QuicStreamSequencerBuffer error: OnStreamData() exceed array bounds."
          "write offset = ",
          offset, " write_block_num = ", write_block_num,
          " blocks_count_ = ", blocks_count_);
      return QUIC_STREAM_SEQUENCER_INVALID_STATE;
    }
    if (blocks_ == nullptr) {
      *error_details =
          "QuicStreamSequencerBuffer error: OnStreamData() blocks_ is null";
      return QUIC_STREAM_SEQUENCER_INVALID_STATE;
    }
    if (blocks_[write_block_num] == nullptr) {
      blocks_[write_block_num] = new BufferBlock();
    }

    const size_t bytes_to_copy =
        std::min<size_t>(bytes_avail, source_remaining);
    char* dest = blocks_[write_block_num]->buffer + write_block_offset;
    QUIC_DVLOG(1) << "Write at offset: " << offset
                  << " length: " << bytes_to_copy;

    if (dest == nullptr || source == nullptr) {
      *error_details = QuicStrCat(
          "QuicStreamSequencerBuffer error: OnStreamData()"
          " dest == nullptr: ",
          (dest == nullptr), " source == nullptr: ", (source == nullptr),
          " Writing at offset ", offset, " Gaps: ", GapsDebugString(),
          " Remaining frames: ", ReceivedFramesDebugString(),
          " total_bytes_read_ = ", total_bytes_read_);
      return QUIC_STREAM_SEQUENCER_INVALID_STATE;
    }
    memcpy(dest, source, bytes_to_copy);
    source += bytes_to_copy;
    source_remaining -= bytes_to_copy;
    offset += bytes_to_copy;
    total_written += bytes_to_copy;
  }

  DCHECK_GT(total_written, 0u);
  *bytes_buffered = total_written;
  UpdateGapList(current_gap, starting_offset, total_written);

  frame_arrival_time_map_.insert(
      std::make_pair(starting_offset, FrameInfo(size, timestamp)));
  num_bytes_buffered_ += total_written;
  return QUIC_NO_ERROR;
}
