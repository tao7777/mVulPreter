 ServiceWorkerHandler::ServiceWorkerHandler()
     : DevToolsDomainHandler(ServiceWorker::Metainfo::domainName),
       enabled_(false),
      browser_context_(nullptr),
      storage_partition_(nullptr),
       weak_factory_(this) {}
