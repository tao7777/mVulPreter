void DownloadFileManager::CompleteDownload(DownloadId global_id) {
void DownloadFileManager::CompleteDownload(
    DownloadId global_id, const base::Closure& callback) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
 
   if (!ContainsKey(downloads_, global_id))
    return;

  DownloadFile* download_file = downloads_[global_id];

  VLOG(20) << " " << __FUNCTION__ << "()"
            << " id = " << global_id
            << " download_file = " << download_file->DebugString();
 
  // Done here on Windows so that anti-virus scanners invoked by
  // the attachment service actually see the data; see
  // http://crbug.com/127999.
  // Done here for mac because we only want to do this once; see
  // http://crbug.com/13120 for details.
  // Other platforms don't currently do source annotation.
  download_file->AnnotateWithSourceInformation();

   download_file->Detach();
 
   EraseDownload(global_id);

  // Notify our caller we've let it go.
  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE, callback);
 }
