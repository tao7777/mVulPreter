bool DownloadItemImpl::CanOpenDownload() {
   const bool is_complete = GetState() == DownloadItem::COMPLETE;
   return (!IsDone() || is_complete) && !IsTemporary() &&
         !file_externally_removed_ &&
         delegate_->IsMostRecentDownloadItemAtFilePath(this);
 }
