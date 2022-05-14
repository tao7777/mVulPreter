 void PPB_URLLoader_Impl::FinishLoading(int32_t done_status) {
   done_status_ = done_status;
  user_buffer_ = NULL;
  user_buffer_size_ = 0;
  if (TrackedCallback::IsPending(pending_callback_))
    RunCallback(done_status_);
}
