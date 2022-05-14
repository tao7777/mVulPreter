void DownloadFileManager::RenameCompletingDownloadFile(
    DownloadId global_id,
    const FilePath& full_path,
    bool overwrite_existing_file,
    const RenameCompletionCallback& callback) {
  VLOG(20) << __FUNCTION__ << "()" << " id = " << global_id
           << " overwrite_existing_file = " << overwrite_existing_file
           << " full_path = \"" << full_path.value() << "\"";
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  DownloadFile* download_file = GetDownloadFile(global_id);
  if (!download_file) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                            base::Bind(callback, FilePath()));
    return;
  }
 
  VLOG(20) << __FUNCTION__ << "()"
           << " download_file = " << download_file->DebugString();
  FilePath new_path = full_path;
  if (!overwrite_existing_file) {
    int uniquifier =
        file_util::GetUniquePathNumber(new_path, FILE_PATH_LITERAL(""));
    if (uniquifier > 0) {
      new_path = new_path.InsertBeforeExtensionASCII(
          StringPrintf(" (%d)", uniquifier));
    }
  }
  net::Error rename_error = download_file->Rename(new_path);
  if (net::OK != rename_error) {
    CancelDownloadOnRename(global_id, rename_error);
     new_path.clear();
  } else {
    download_file->AnnotateWithSourceInformation();
   }
   BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                           base::Bind(callback, new_path));
 }
