 NetworkHandler::NetworkHandler(const std::string& host_id)
     : DevToolsDomainHandler(Network::Metainfo::domainName),
      process_(nullptr),
       host_(nullptr),
       enabled_(false),
       host_id_(host_id),
      bypass_service_worker_(false),
      cache_disabled_(false),
      weak_factory_(this) {
  static bool have_configured_service_worker_context = false;
  if (have_configured_service_worker_context)
    return;
  have_configured_service_worker_context = true;
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                          base::BindOnce(&ConfigureServiceWorkerContextOnIO));
}
