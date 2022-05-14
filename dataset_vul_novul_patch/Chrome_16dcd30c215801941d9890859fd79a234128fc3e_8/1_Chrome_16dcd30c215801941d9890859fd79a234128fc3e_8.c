void DownloadItemImpl::OnDownloadRenamedToFinalName(
    DownloadFileManager* file_manager,
    const FilePath& full_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  VLOG(20) << __FUNCTION__ << "()"
           << " full_path = \"" << full_path.value() << "\""
           << " needed rename = " << NeedsRename()
            << " " << DebugString(false);
   DCHECK(NeedsRename());
 
  if (!full_path.empty()) {
    target_path_ = full_path;
    SetFullPath(full_path);
    delegate_->DownloadRenamedToFinalName(this);
 
    if (delegate_->ShouldOpenDownload(this))
      Completed();
    else
      delegate_delayed_complete_ = true;
 
    BrowserThread::PostTask(
        BrowserThread::FILE, FROM_HERE,
        base::Bind(&DownloadFileManager::CompleteDownload,
                   file_manager, GetGlobalId()));
  }
 }
