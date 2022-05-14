 IOHandler::IOHandler(DevToolsIOContext* io_context)
     : DevToolsDomainHandler(IO::Metainfo::domainName),
       io_context_(io_context),
      process_host_(nullptr),
       weak_factory_(this) {}
