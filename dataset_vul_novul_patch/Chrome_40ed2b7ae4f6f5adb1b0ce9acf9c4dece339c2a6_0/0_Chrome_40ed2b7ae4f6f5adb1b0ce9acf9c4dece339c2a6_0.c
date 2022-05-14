GDataRootDirectory::GDataRootDirectory()
    : ALLOW_THIS_IN_INITIALIZER_LIST(GDataDirectory(NULL, this)),
      fake_search_directory_(new GDataDirectory(NULL, NULL)),
       largest_changestamp_(0), serialized_size_(0) {
   title_ = kGDataRootDirectory;
   SetFileNameFromTitle();
  resource_id_ = kGDataRootDirectoryResourceId;
  // Add self to the map so the root directory can be looked up by the
  // resource ID.
  AddEntryToResourceMap(this);
 }
