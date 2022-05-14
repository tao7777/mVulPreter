GDataFileError GDataFileSystem::AddNewDirectory(
    const FilePath& directory_path, base::Value* entry_value) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (!entry_value)
    return GDATA_FILE_ERROR_FAILED;

  scoped_ptr<DocumentEntry> doc_entry(DocumentEntry::CreateFrom(*entry_value));

  if (!doc_entry.get())
    return GDATA_FILE_ERROR_FAILED;

  GDataEntry* entry = directory_service_->FindEntryByPathSync(directory_path);
  if (!entry)
    return GDATA_FILE_ERROR_FAILED;

  GDataDirectory* parent_dir = entry->AsGDataDirectory();
   if (!parent_dir)
     return GDATA_FILE_ERROR_FAILED;
 
  GDataEntry* new_entry = GDataEntry::FromDocumentEntry(
      NULL, doc_entry.get(), directory_service_.get());
   if (!new_entry)
     return GDATA_FILE_ERROR_FAILED;
 
  parent_dir->AddEntry(new_entry);

  OnDirectoryChanged(directory_path);
  return GDATA_FILE_OK;
}
