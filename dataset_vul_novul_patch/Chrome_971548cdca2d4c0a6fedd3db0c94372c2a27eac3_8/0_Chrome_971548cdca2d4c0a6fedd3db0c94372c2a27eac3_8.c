 MediaStreamDispatcherHost::~MediaStreamDispatcherHost() {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
   CancelAllRequests();
 }
