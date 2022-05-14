GDataFileError GDataWapiFeedProcessor::FeedToFileResourceMap(
    const std::vector<DocumentFeed*>& feed_list,
    FileResourceIdMap* file_map,
    int64* feed_changestamp,
    FeedToFileResourceMapUmaStats* uma_stats) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(uma_stats);

  GDataFileError error = GDATA_FILE_OK;
  uma_stats->num_regular_files = 0;
  uma_stats->num_hosted_documents = 0;
  uma_stats->num_files_with_entry_kind.clear();
  for (size_t i = 0; i < feed_list.size(); ++i) {
    const DocumentFeed* feed = feed_list[i];

    if (i == 0) {
      const Link* root_feed_upload_link =
          feed->GetLinkByType(Link::RESUMABLE_CREATE_MEDIA);
      if (root_feed_upload_link)
        directory_service_->root()->set_upload_url(
            root_feed_upload_link->href());
      *feed_changestamp = feed->largest_changestamp();
      DCHECK_GE(*feed_changestamp, 0);
    }

    for (ScopedVector<DocumentEntry>::const_iterator iter =
              feed->entries().begin();
          iter != feed->entries().end(); ++iter) {
       DocumentEntry* doc = *iter;
      GDataEntry* entry = GDataEntry::FromDocumentEntry(
          NULL, doc, directory_service_);
       if (!entry)
         continue;
      GDataFile* as_file = entry->AsGDataFile();
      if (as_file) {
        if (as_file->is_hosted_document())
          ++uma_stats->num_hosted_documents;
        else
          ++uma_stats->num_regular_files;
        ++uma_stats->num_files_with_entry_kind[as_file->kind()];
      }

      FileResourceIdMap::iterator map_entry =
          file_map->find(entry->resource_id());

      if (map_entry != file_map->end()) {
        LOG(WARNING) << "Found duplicate file "
                     << map_entry->second->base_name();

        delete map_entry->second;
        file_map->erase(map_entry);
      }
      file_map->insert(
          std::pair<std::string, GDataEntry*>(entry->resource_id(), entry));
    }
  }

  if (error != GDATA_FILE_OK) {
    STLDeleteValues(file_map);
  }

  return error;
}
