 void QuicStreamHost::Finish() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   DCHECK(p2p_stream_);
  p2p_stream_->Finish();
   writeable_ = false;
   if (!readable_ && !writeable_) {
     Delete();
  }
}
