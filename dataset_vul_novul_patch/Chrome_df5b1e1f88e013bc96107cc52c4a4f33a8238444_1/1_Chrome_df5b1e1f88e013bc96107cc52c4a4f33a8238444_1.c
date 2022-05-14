 void BackendImpl::OnEntryDestroyEnd() {
   DecreaseNumRefs();
  if (data_->header.num_bytes > max_size_ && !read_only_ &&
      (up_ticks_ > kTrimDelay || user_flags_ & kNoRandom))
    eviction_.TrimCache(false);
 }
