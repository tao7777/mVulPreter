void DownloadFileManager::RenameInProgressDownloadFile(
void DownloadFileManager::RenameDownloadFile(
     DownloadId global_id,
     const FilePath& full_path,
     bool overwrite_existing_file,
     const RenameCompletionCallback& callback) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
   DownloadFile* download_file = GetDownloadFile(global_id);
   if (!download_file) {
     BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                             base::Bind(callback, FilePath()));
     return;
   }
 
   FilePath new_path(full_path);
   if (!overwrite_existing_file) {
    // Make the file unique if requested.
     int uniquifier =
         file_util::GetUniquePathNumber(new_path, FILE_PATH_LITERAL(""));
     if (uniquifier > 0) {
      new_path = new_path.InsertBeforeExtensionASCII(
          StringPrintf(" (%d)", uniquifier));
     }
   }
 
  // Do the actual rename
   net::Error rename_error = download_file->Rename(new_path);
   if (net::OK != rename_error) {
