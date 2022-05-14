void DownloadFileManager::CancelDownloadOnRename(
    DownloadId global_id, net::Error rename_error) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  DownloadFile* download_file = GetDownloadFile(global_id);
  if (!download_file)
    return;
  DownloadManager* download_manager = download_file->GetDownloadManager();
  DCHECK(download_manager);
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&DownloadManager::OnDownloadInterrupted,
                 download_manager,
                 global_id.local(),
                 download_file->BytesSoFar(),
                 download_file->GetHashState(),
                 content::ConvertNetErrorToInterruptReason(
                     rename_error,
                     content::DOWNLOAD_INTERRUPT_FROM_DISK)));
}
