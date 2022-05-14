int HttpStreamParser::DoReadHeadersComplete(int result) {
  if (result == 0)
    result = ERR_CONNECTION_CLOSED;

  if (result < 0 && result != ERR_CONNECTION_CLOSED) {
    io_state_ = STATE_DONE;
    return result;
  }
  if (result == ERR_CONNECTION_CLOSED && read_buf_->offset() == 0 &&
      connection_->is_reused()) {
    io_state_ = STATE_DONE;
    return result;
  }

  if (read_buf_->offset() == 0 && result != ERR_CONNECTION_CLOSED)
    response_->response_time = base::Time::Now();
 
   if (result == ERR_CONNECTION_CLOSED) {
       io_state_ = STATE_DONE;
       return ERR_EMPTY_RESPONSE;
    } else if (request_->url.SchemeIs("https")) {
      // The connection was closed in the middle of the headers. For HTTPS we
      // don't parse partial headers. Return a different error code so that we
      // know that we shouldn't attempt to retry the request.
      io_state_ = STATE_DONE;
      return ERR_HEADERS_TRUNCATED;
    }
    // Parse things as well as we can and let the caller decide what to do.
    int end_offset;
    if (response_header_start_offset_ >= 0) {
      io_state_ = STATE_READ_BODY_COMPLETE;
      end_offset = read_buf_->offset();
     } else {
      io_state_ = STATE_BODY_PENDING;
      end_offset = 0;
     }
    int rv = DoParseResponseHeaders(end_offset);
    if (rv < 0)
      return rv;
    return result;
   }
 
   read_buf_->set_offset(read_buf_->offset() + result);
  DCHECK_LE(read_buf_->offset(), read_buf_->capacity());
  DCHECK_GE(result,  0);

  int end_of_header_offset = ParseResponseHeaders();

  if (end_of_header_offset < -1)
    return end_of_header_offset;

  if (end_of_header_offset == -1) {
    io_state_ = STATE_READ_HEADERS;
    if (read_buf_->offset() - read_buf_unused_offset_ >= kMaxHeaderBufSize) {
      io_state_ = STATE_DONE;
      return ERR_RESPONSE_HEADERS_TOO_BIG;
    }
  } else {
    read_buf_unused_offset_ = end_of_header_offset;

    if (response_->headers->response_code() / 100 == 1) {
      io_state_ = STATE_REQUEST_SENT;
      response_header_start_offset_ = -1;
    } else {
      io_state_ = STATE_BODY_PENDING;
      CalculateResponseBodySize();
      if (response_body_length_ == 0) {
        io_state_ = STATE_DONE;
        int extra_bytes = read_buf_->offset() - read_buf_unused_offset_;
        if (extra_bytes) {
          CHECK_GT(extra_bytes, 0);
          memmove(read_buf_->StartOfBuffer(),
                  read_buf_->StartOfBuffer() + read_buf_unused_offset_,
                  extra_bytes);
        }
        read_buf_->SetCapacity(extra_bytes);
        read_buf_unused_offset_ = 0;
        return OK;
      }
    }
  }
  return result;
}
