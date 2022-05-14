 StorageHandler::StorageHandler()
     : DevToolsDomainHandler(Storage::Metainfo::domainName),
      storage_partition_(nullptr),
       weak_ptr_factory_(this) {}
