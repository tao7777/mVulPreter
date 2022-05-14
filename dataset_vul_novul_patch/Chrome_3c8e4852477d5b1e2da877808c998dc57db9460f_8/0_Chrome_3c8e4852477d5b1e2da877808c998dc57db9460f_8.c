 IOHandler::IOHandler(DevToolsIOContext* io_context)
     : DevToolsDomainHandler(IO::Metainfo::domainName),
       io_context_(io_context),
      browser_context_(nullptr),
      storage_partition_(nullptr),
       weak_factory_(this) {}
