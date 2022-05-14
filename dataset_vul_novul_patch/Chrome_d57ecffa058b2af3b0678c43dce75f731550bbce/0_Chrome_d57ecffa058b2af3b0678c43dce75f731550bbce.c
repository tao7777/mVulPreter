void ChangeListLoader::LoadAfterGetAboutResource(
    const DirectoryFetchInfo& directory_fetch_info,
    bool is_initial_load,
    int64 local_changestamp,
    google_apis::GDataErrorCode status,
    scoped_ptr<google_apis::AboutResource> about_resource) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
 
   FileError error = GDataToFileError(status);
   if (error != FILE_ERROR_OK) {
    if (directory_fetch_info.empty() || is_initial_load)
      OnChangeListLoadComplete(error);
    else
      OnDirectoryLoadComplete(directory_fetch_info, error);
     return;
   }
 
   DCHECK(about_resource);
 
   int64 remote_changestamp = about_resource->largest_change_id();
   int64 start_changestamp = local_changestamp > 0 ? local_changestamp + 1 : 0;
   if (directory_fetch_info.empty()) {
    if (local_changestamp >= remote_changestamp) {
      if (local_changestamp > remote_changestamp) {
        LOG(WARNING) << "Local resource metadata is fresher than server, "
                     << "local = " << local_changestamp
                     << ", server = " << remote_changestamp;
      }

      // No changes detected, tell the client that the loading was successful.
      OnChangeListLoadComplete(FILE_ERROR_OK);
      return;
    }

     LoadChangeListFromServer(start_changestamp);
  } else {
    int64 directory_changestamp = std::max(directory_fetch_info.changestamp(),
                                           local_changestamp);

    util::Log(logging::LOG_INFO,
              "Fast-fetch start: %s; Server changestamp: %s",
              directory_fetch_info.ToString().c_str(),
              base::Int64ToString(remote_changestamp).c_str());

    if (directory_changestamp >= remote_changestamp) {
      LoadAfterLoadDirectory(directory_fetch_info, is_initial_load,
                             start_changestamp, FILE_ERROR_OK);
      return;
    }

    DirectoryFetchInfo new_directory_fetch_info(
        directory_fetch_info.local_id(), directory_fetch_info.resource_id(),
        remote_changestamp);
    LoadDirectoryFromServer(
        new_directory_fetch_info,
        base::Bind(&ChangeListLoader::LoadAfterLoadDirectory,
                   weak_ptr_factory_.GetWeakPtr(),
                   directory_fetch_info,
                   is_initial_load,
                   start_changestamp));
  }
}
