 void PPB_URLLoader_Impl::FinishLoading(int32_t done_status) {
   done_status_ = done_status;
  if (TrackedCallback::IsPending(pending_callback_))
    RunCallback(done_status_);
}
