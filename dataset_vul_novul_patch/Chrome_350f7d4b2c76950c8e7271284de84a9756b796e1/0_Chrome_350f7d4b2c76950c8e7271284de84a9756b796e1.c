void P2PQuicStreamImpl::Finish() {
void P2PQuicStreamImpl::WriteData(std::vector<uint8_t> data, bool fin) {
  // It is up to the delegate to not write more data than the
  // |write_buffer_size_|.
  DCHECK_GE(write_buffer_size_, data.size() + write_buffered_amount_);
  write_buffered_amount_ += data.size();
  QuicStream::WriteOrBufferData(
      quic::QuicStringPiece(reinterpret_cast<const char*>(data.data()),
                            data.size()),
      fin, nullptr);
 }
