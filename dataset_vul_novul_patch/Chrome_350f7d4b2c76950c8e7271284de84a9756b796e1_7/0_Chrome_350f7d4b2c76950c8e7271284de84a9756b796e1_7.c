 void QuicStreamHost::Finish() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(p2p_stream_);
  std::vector<uint8_t> data;
  p2p_stream_->WriteData(data, true);
   writeable_ = false;
   if (!readable_ && !writeable_) {
     Delete();
  }
}
