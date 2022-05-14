void FileAPIMessageFilter::OnCreateSnapshotFile(
    int request_id, const GURL& blob_url, const GURL& path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
   FileSystemURL url(path);
   base::Callback<void(const FilePath&)> register_file_callback =
       base::Bind(&FileAPIMessageFilter::RegisterFileAsBlob,
                 this, blob_url, url.path());
 
   FileSystemOperation* operation = GetNewOperation(url, request_id);
   if (!operation)
    return;
  operation->CreateSnapshotFile(
      url,
      base::Bind(&FileAPIMessageFilter::DidCreateSnapshot,
                 this, request_id, register_file_callback));
}
