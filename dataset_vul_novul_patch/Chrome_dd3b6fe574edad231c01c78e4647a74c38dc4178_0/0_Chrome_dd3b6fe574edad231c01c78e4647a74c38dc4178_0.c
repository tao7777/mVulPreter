void GDataFileSystem::AddUploadedFileOnUIThread(
    UploadMode upload_mode,
    const FilePath& virtual_dir_path,
    scoped_ptr<DocumentEntry> entry,
    const FilePath& file_content_path,
    GDataCache::FileOperationType cache_operation,
    const base::Closure& callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  base::ScopedClosureRunner callback_runner(callback);

  if (!entry.get()) {
    NOTREACHED();
    return;
  }

  GDataEntry* dir_entry = directory_service_->FindEntryByPathSync(
      virtual_dir_path);
  if (!dir_entry)
    return;

  GDataDirectory* parent_dir  = dir_entry->AsGDataDirectory();
  if (!parent_dir)
     return;
 
   scoped_ptr<GDataEntry> new_entry(
      directory_service_->FromDocumentEntry(entry.get()));
   if (!new_entry.get())
     return;
 
  if (upload_mode == UPLOAD_EXISTING_FILE) {
    const std::string& resource_id = new_entry->resource_id();
    directory_service_->GetEntryByResourceIdAsync(resource_id,
        base::Bind(&RemoveStaleEntryOnUpload, resource_id, parent_dir));
  }

  GDataFile* file = new_entry->AsGDataFile();
  DCHECK(file);
  const std::string& resource_id = file->resource_id();
  const std::string& md5 = file->file_md5();
  parent_dir->AddEntry(new_entry.release());

  OnDirectoryChanged(virtual_dir_path);

  if (upload_mode == UPLOAD_NEW_FILE) {
    cache_->StoreOnUIThread(resource_id,
                            md5,
                            file_content_path,
                            cache_operation,
                            base::Bind(&OnCacheUpdatedForAddUploadedFile,
                                       callback_runner.Release()));
  } else if (upload_mode == UPLOAD_EXISTING_FILE) {
    cache_->ClearDirtyOnUIThread(resource_id,
                                 md5,
                                 base::Bind(&OnCacheUpdatedForAddUploadedFile,
                                            callback_runner.Release()));
  } else {
    NOTREACHED() << "Unexpected upload mode: " << upload_mode;
  }
}
