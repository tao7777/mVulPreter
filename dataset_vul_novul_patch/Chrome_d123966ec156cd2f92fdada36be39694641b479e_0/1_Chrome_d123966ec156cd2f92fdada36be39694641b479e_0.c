 void FileAPIMessageFilter::RegisterFileAsBlob(const GURL& blob_url,
                                              const FilePath& virtual_path,
                                               const FilePath& platform_path) {
  FilePath::StringType extension = virtual_path.Extension();
   if (!extension.empty())
     extension = extension.substr(1);  // Strip leading ".".
 
   scoped_refptr<webkit_blob::ShareableFileReference> shareable_file =
       webkit_blob::ShareableFileReference::Get(platform_path);
  if (shareable_file &&
      !ChildProcessSecurityPolicyImpl::GetInstance()->CanReadFile(
           process_id_, platform_path)) {
     ChildProcessSecurityPolicyImpl::GetInstance()->GrantReadFile(
         process_id_, platform_path);
    shareable_file->AddFinalReleaseCallback(
        base::Bind(&RevokeFilePermission, process_id_));
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
