 int BlobURLRequestJob::ComputeBytesToRead() const {
  int64 current_item_length = item_length_list_[current_item_index_];

  int64 item_remaining = current_item_length - current_item_offset_;
  int64 buf_remaining = read_buf_->BytesRemaining();
  int64 max_remaining = std::numeric_limits<int>::max();

  int64 min = std::min(std::min(std::min(item_remaining,
                                         buf_remaining),
                                         remaining_bytes_),
                                         max_remaining);

  return static_cast<int>(min);
 }
