void GDataFileSystem::OnCopyDocumentCompleted(
    const FilePath& dir_path,
    const FileOperationCallback& callback,
    GDataErrorCode status,
    scoped_ptr<base::Value> data) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(!callback.is_null());

  GDataFileError error = util::GDataToGDataFileError(status);
  if (error != GDATA_FILE_OK) {
    callback.Run(error);
    return;
  }

  scoped_ptr<DocumentEntry> doc_entry(DocumentEntry::ExtractAndParse(*data));
  if (!doc_entry.get()) {
    callback.Run(GDATA_FILE_ERROR_FAILED);
     return;
   }
 
  GDataEntry* entry = GDataEntry::FromDocumentEntry(
      NULL, doc_entry.get(), directory_service_.get());
   if (!entry) {
     callback.Run(GDATA_FILE_ERROR_FAILED);
     return;
  }

  directory_service_->root()->AddEntry(entry);
  MoveEntryFromRootDirectory(dir_path,
                             callback,
                             GDATA_FILE_OK,
                             entry->GetFilePath());
}
