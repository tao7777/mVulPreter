void GDataFileSystem::OnGetDocumentEntry(const FilePath& cache_file_path,
                                         const GetFileFromCacheParams& params,
                                         GDataErrorCode status,
                                         scoped_ptr<base::Value> data) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  GDataFileError error = util::GDataToGDataFileError(status);

  scoped_ptr<GDataEntry> fresh_entry;
   if (error == GDATA_FILE_OK) {
     scoped_ptr<DocumentEntry> doc_entry(DocumentEntry::ExtractAndParse(*data));
     if (doc_entry.get()) {
      fresh_entry.reset(
          GDataEntry::FromDocumentEntry(NULL, doc_entry.get(),
                                        directory_service_.get()));
     }
     if (!fresh_entry.get() || !fresh_entry->AsGDataFile()) {
       LOG(ERROR) << "Got invalid entry from server for " << params.resource_id;
      error = GDATA_FILE_ERROR_FAILED;
    }
  }

  if (error != GDATA_FILE_OK) {
    if (!params.get_file_callback.is_null()) {
      params.get_file_callback.Run(error,
                                   cache_file_path,
                                   params.mime_type,
                                   REGULAR_FILE);
    }
    return;
  }

  GURL content_url = fresh_entry->content_url();
  int64 file_size = fresh_entry->file_info().size;

  DCHECK_EQ(params.resource_id, fresh_entry->resource_id());
  scoped_ptr<GDataFile> fresh_entry_as_file(
      fresh_entry.release()->AsGDataFile());
  directory_service_->RefreshFile(fresh_entry_as_file.Pass());

  bool* has_enough_space = new bool(false);
  util::PostBlockingPoolSequencedTaskAndReply(
      FROM_HERE,
      blocking_task_runner_,
      base::Bind(&GDataCache::FreeDiskSpaceIfNeededFor,
                 base::Unretained(cache_),
                 file_size,
                 has_enough_space),
      base::Bind(&GDataFileSystem::StartDownloadFileIfEnoughSpace,
                 ui_weak_ptr_,
                 params,
                 content_url,
                 cache_file_path,
                 base::Owned(has_enough_space)));
}
