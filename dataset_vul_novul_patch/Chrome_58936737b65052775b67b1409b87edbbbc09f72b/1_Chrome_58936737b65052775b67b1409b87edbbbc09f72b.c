 int BlobURLRequestJob::ComputeBytesToRead() const {
  int64 current_item_remaining_bytes =
      item_length_list_[current_item_index_] - current_item_offset_;
  int64 remaining_bytes = std::min(current_item_remaining_bytes,
                                   remaining_bytes_);
  return static_cast<int>(std::min(
             static_cast<int64>(read_buf_->BytesRemaining()),
             remaining_bytes));
 }
