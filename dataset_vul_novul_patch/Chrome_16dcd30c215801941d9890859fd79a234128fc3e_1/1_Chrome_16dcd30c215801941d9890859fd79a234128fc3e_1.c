void DownloadFileManager::CompleteDownload(DownloadId global_id) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
 
   if (!ContainsKey(downloads_, global_id))
    return;

  DownloadFile* download_file = downloads_[global_id];

  VLOG(20) << " " << __FUNCTION__ << "()"
            << " id = " << global_id
            << " download_file = " << download_file->DebugString();
 
   download_file->Detach();
 
   EraseDownload(global_id);
 }
