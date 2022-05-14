void ChunkedUploadDataStream::AppendData(
    const char* data, int data_len, bool is_done) {
  DCHECK(!all_data_appended_);
  DCHECK(data_len > 0 || is_done);
   if (data_len > 0) {
     DCHECK(data);
     upload_data_.push_back(
        std::make_unique<std::vector<char>>(data, data + data_len));
   }
   all_data_appended_ = is_done;
 
  if (!read_buffer_.get())
    return;

  int result = ReadChunk(read_buffer_.get(), read_buffer_len_);
  DCHECK_GE(result, 0);
  read_buffer_ = NULL;
  read_buffer_len_ = 0;
  OnReadCompleted(result);
}
