 void FileAPIMessageFilter::RegisterFileAsBlob(const GURL& blob_url,
                                              const FileSystemURL& url,
                                               const FilePath& platform_path) {
  FilePath::StringType extension = url.path().Extension();
   if (!extension.empty())
     extension = extension.substr(1);  // Strip leading ".".
 
   scoped_refptr<webkit_blob::ShareableFileReference> shareable_file =
       webkit_blob::ShareableFileReference::Get(platform_path);
  if (!ChildProcessSecurityPolicyImpl::GetInstance()->CanReadFile(
           process_id_, platform_path)) {
    // If the underlying file system implementation is returning a new
    // (likely temporary) snapshot file or the file is for sandboxed
    // filesystems it's ok to grant permission here.
    // (Note that we have also already checked if the renderer has the
    // read permission for this file in OnCreateSnapshotFile.)
    DCHECK(shareable_file ||
           fileapi::SandboxMountPointProvider::CanHandleType(url.type()));
     ChildProcessSecurityPolicyImpl::GetInstance()->GrantReadFile(
         process_id_, platform_path);
    if (shareable_file) {
      // This will revoke all permissions for the file when the last ref
      // of the file is dropped (assuming it's ok).
      shareable_file->AddFinalReleaseCallback(
          base::Bind(&RevokeFilePermission, process_id_));
    }
   }
 
  std::string mime_type;
  net::GetWellKnownMimeTypeFromExtension(extension, &mime_type);
  BlobData::Item item;
  item.SetToFilePathRange(platform_path, 0, -1, base::Time());
  BlobStorageController* controller = blob_storage_context_->controller();
  controller->StartBuildingBlob(blob_url);
  controller->AppendBlobDataItem(blob_url, item);
  controller->FinishBuildingBlob(blob_url, mime_type);
  blob_urls_.insert(blob_url.spec());
}
