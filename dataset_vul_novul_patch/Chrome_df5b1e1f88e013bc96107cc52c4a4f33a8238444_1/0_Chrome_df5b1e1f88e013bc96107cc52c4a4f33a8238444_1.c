 void BackendImpl::OnEntryDestroyEnd() {
   DecreaseNumRefs();
  consider_evicting_at_op_end_ = true;
}

void BackendImpl::OnSyncBackendOpComplete() {
  if (consider_evicting_at_op_end_) {
    if (data_->header.num_bytes > max_size_ && !read_only_ &&
        (up_ticks_ > kTrimDelay || user_flags_ & kNoRandom))
      eviction_.TrimCache(false);
    consider_evicting_at_op_end_ = false;
  }
 }
