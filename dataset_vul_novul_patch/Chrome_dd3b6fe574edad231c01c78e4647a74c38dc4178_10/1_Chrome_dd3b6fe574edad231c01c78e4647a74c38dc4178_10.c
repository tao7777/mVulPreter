GDataDirectoryService::GDataDirectoryService()
    : blocking_task_runner_(NULL),
      serialized_size_(0),
       largest_changestamp_(0),
       origin_(UNINITIALIZED),
       weak_ptr_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
  root_.reset(new GDataDirectory(NULL, this));
   if (!util::IsDriveV2ApiEnabled())
     InitializeRootEntry(kGDataRootDirectoryResourceId);
 }
