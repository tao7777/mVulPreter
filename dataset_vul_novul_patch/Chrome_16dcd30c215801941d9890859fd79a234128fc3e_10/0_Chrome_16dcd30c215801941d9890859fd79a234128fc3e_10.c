 void DownloadItemImpl::UpdateProgress(int64 bytes_so_far,
                                      int64 bytes_per_sec,
                                       const std::string& hash_state) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
 
  if (!IsInProgress()) {
    // Ignore if we're no longer in-progress.  This can happen if we race a
    // Cancel on the UI thread with an update on the FILE thread.
    //
    // TODO(rdsmith): Arguably we should let this go through, as this means
    // the download really did get further than we know before it was
    // cancelled.  But the gain isn't very large, and the code is more
    // fragile if it has to support in progress updates in a non-in-progress
    // state.  This issue should be readdressed when we revamp performance
    // reporting.
    return;
  }
  bytes_per_sec_ = bytes_per_sec;
  hash_state_ = hash_state;
   received_bytes_ = bytes_so_far;
 
  if (received_bytes_ > total_bytes_)
    total_bytes_ = 0;

  if (bound_net_log_.IsLoggingAllEvents()) {
    bound_net_log_.AddEvent(
         net::NetLog::TYPE_DOWNLOAD_ITEM_UPDATED,
         net::NetLog::Int64Callback("bytes_so_far", received_bytes_));
   }
