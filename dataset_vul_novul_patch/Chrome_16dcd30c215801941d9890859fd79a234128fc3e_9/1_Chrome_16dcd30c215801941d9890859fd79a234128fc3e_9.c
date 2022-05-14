void DownloadItemImpl::OnIntermediatePathDetermined(
    DownloadFileManager* file_manager,
    const FilePath& intermediate_path,
    bool ok_to_overwrite) {
  DownloadFileManager::RenameCompletionCallback callback =
      base::Bind(&DownloadItemImpl::OnDownloadRenamedToIntermediateName,
                  weak_ptr_factory_.GetWeakPtr());
   BrowserThread::PostTask(
       BrowserThread::FILE, FROM_HERE,
      base::Bind(&DownloadFileManager::RenameInProgressDownloadFile,
                  file_manager, GetGlobalId(), intermediate_path,
                  ok_to_overwrite, callback));
 }
