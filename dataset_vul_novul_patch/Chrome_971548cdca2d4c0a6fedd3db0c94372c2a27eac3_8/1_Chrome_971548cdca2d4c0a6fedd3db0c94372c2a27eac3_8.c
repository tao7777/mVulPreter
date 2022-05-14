 MediaStreamDispatcherHost::~MediaStreamDispatcherHost() {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  bindings_.CloseAllBindings();
   CancelAllRequests();
 }
