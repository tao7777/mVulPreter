void FileAPIMessageFilter::OnCreateSnapshotFile(
    int request_id, const GURL& blob_url, const GURL& path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
   FileSystemURL url(path);
   base::Callback<void(const FilePath&)> register_file_callback =
       base::Bind(&FileAPIMessageFilter::RegisterFileAsBlob,
                 this, blob_url, url);

  // Make sure if this file can be read by the renderer as this is
  // called when the renderer is about to create a new File object
  // (for reading the file).
  base::PlatformFileError error;
  if (!HasPermissionsForFile(url, kReadFilePermissions, &error)) {
    Send(new FileSystemMsg_DidFail(request_id, error));
    return;
  }
 
   FileSystemOperation* operation = GetNewOperation(url, request_id);
   if (!operation)
    return;
  operation->CreateSnapshotFile(
      url,
      base::Bind(&FileAPIMessageFilter::DidCreateSnapshot,
                 this, request_id, register_file_callback));
}
