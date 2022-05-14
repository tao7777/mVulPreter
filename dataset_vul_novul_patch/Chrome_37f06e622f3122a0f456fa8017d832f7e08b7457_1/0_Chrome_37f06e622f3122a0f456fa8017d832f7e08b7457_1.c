 bool HpackDecoder::HandleControlFrameHeadersComplete(SpdyStreamId id) {
   HpackInputStream input_stream(max_string_literal_size_,
                                 headers_block_buffer_);
  regular_header_seen_ = false;
   while (input_stream.HasMoreData()) {
     if (!DecodeNextOpcode(&input_stream)) {
       headers_block_buffer_.clear();
      return false;
    }
  }
  headers_block_buffer_.clear();

  if (!cookie_value_.empty()) {
    decoded_block_[kCookieKey] = cookie_value_;
    cookie_value_.clear();
  }
   return true;
 }
