 WebRunnerBrowserContext::~WebRunnerBrowserContext() {
  NotifyWillBeDestroyed(this);

   if (resource_context_) {
     content::BrowserThread::DeleteSoon(content::BrowserThread::IO, FROM_HERE,
                                        std::move(resource_context_));
   }

  ShutdownStoragePartitions();
 }
