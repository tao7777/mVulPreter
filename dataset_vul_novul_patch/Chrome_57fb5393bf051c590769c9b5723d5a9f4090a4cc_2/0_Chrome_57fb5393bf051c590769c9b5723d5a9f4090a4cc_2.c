int NetworkReaderProxy::Read(net::IOBuffer* buffer, int buffer_length,
                             const net::CompletionCallback& callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  DCHECK(!buffer_);
  DCHECK_EQ(buffer_length_, 0);
  DCHECK(callback_.is_null());
  DCHECK(buffer);
  DCHECK_GT(buffer_length, 0);
  DCHECK(!callback.is_null());

  if (error_code_ != net::OK) {
    return error_code_;
  }

  if (remaining_content_length_ == 0) {
     return 0;
   }
 
  if (buffer_length > remaining_content_length_) {
    // Here, narrowing cast should be safe.
    buffer_length = static_cast<int>(remaining_content_length_);
  }

   if (pending_data_.empty()) {
     buffer_ = buffer;
    buffer_length_ = buffer_length;
    callback_ = callback;
    return net::ERR_IO_PENDING;
  }

  int result = ReadInternal(&pending_data_, buffer, buffer_length);
  remaining_content_length_ -= result;
  DCHECK_GE(remaining_content_length_, 0);
  return result;
}
