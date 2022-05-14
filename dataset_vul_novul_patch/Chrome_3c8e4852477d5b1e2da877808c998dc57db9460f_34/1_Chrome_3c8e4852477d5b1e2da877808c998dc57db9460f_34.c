 StorageHandler::StorageHandler()
     : DevToolsDomainHandler(Storage::Metainfo::domainName),
      process_(nullptr),
       weak_ptr_factory_(this) {}
