void DownloadItemImpl::UpdateProgress(int64 bytes_so_far,
                                      int64 bytes_per_sec,
                                      const std::string& hash_state) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
 
  if (!IsInProgress()) {
    NOTREACHED();
    return;
  }
  bytes_per_sec_ = bytes_per_sec;
  UpdateProgress(bytes_so_far, hash_state);
   UpdateObservers();
 }
