void GDataFileSystem::OnSearch(const SearchCallback& callback,
                               GetDocumentsParams* params,
                               GDataFileError error) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (error != GDATA_FILE_OK) {
    if (!callback.is_null())
      callback.Run(error, GURL(), scoped_ptr<std::vector<SearchResultInfo> >());
    return;
  }

  std::vector<SearchResultInfo>* results(new std::vector<SearchResultInfo>());

  DCHECK_EQ(1u, params->feed_list->size());
  DocumentFeed* feed = params->feed_list->at(0);

  GURL next_feed;
  feed->GetNextFeedURL(&next_feed);

  if (feed->entries().empty()) {
    scoped_ptr<std::vector<SearchResultInfo> > result_vec(results);
    if (!callback.is_null())
      callback.Run(error, next_feed, result_vec.Pass());
    return;
  }

   for (size_t i = 0; i < feed->entries().size(); ++i) {
     DocumentEntry* doc = const_cast<DocumentEntry*>(feed->entries()[i]);
    scoped_ptr<GDataEntry> entry(directory_service_->FromDocumentEntry(doc));
 
     if (!entry.get())
       continue;

    DCHECK_EQ(doc->resource_id(), entry->resource_id());
    DCHECK(!entry->is_deleted());

    std::string entry_resource_id = entry->resource_id();

    if (entry->AsGDataFile()) {
      scoped_ptr<GDataFile> entry_as_file(entry.release()->AsGDataFile());
      directory_service_->RefreshFile(entry_as_file.Pass());
      DCHECK(!entry.get());
    }

    directory_service_->GetEntryByResourceIdAsync(entry_resource_id,
        base::Bind(&AddEntryToSearchResults,
                   results,
                   callback,
                   base::Bind(&GDataFileSystem::CheckForUpdates, ui_weak_ptr_),
                   error,
                   i+1 == feed->entries().size(),
                   next_feed));
  }
}
