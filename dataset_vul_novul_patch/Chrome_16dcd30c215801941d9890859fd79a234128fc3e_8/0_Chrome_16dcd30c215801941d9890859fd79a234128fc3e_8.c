void DownloadItemImpl::OnDownloadRenamedToFinalName(
    DownloadFileManager* file_manager,
    const FilePath& full_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  VLOG(20) << __FUNCTION__ << "()"
           << " full_path = \"" << full_path.value() << "\""
           << " needed rename = " << NeedsRename()
            << " " << DebugString(false);
   DCHECK(NeedsRename());
 
  if (full_path.empty())
    // Indicates error; also reported
    // by DownloadManagerImpl::OnDownloadInterrupted.
    return;
 
  // full_path is now the current and target file path.
  target_path_ = full_path;
  SetFullPath(full_path);
  delegate_->DownloadRenamedToFinalName(this);
 
  // Complete the download and release the DownloadFile.
  BrowserThread::PostTask(
      BrowserThread::FILE, FROM_HERE,
      base::Bind(&DownloadFileManager::CompleteDownload,
                 file_manager, GetGlobalId(),
                 base::Bind(&DownloadItemImpl::OnDownloadFileReleased,
                            weak_ptr_factory_.GetWeakPtr())));
}

void DownloadItemImpl::OnDownloadFileReleased() {
  if (delegate_->ShouldOpenDownload(this))
    Completed();
  else
    delegate_delayed_complete_ = true;
 }
