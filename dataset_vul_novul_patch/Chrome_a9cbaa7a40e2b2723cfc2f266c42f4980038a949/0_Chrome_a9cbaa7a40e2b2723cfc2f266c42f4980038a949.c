bool WebMediaPlayerMS::DidGetOpaqueResponseFromServiceWorker() const {
bool WebMediaPlayerMS::WouldTaintOrigin() const {
   DCHECK(thread_checker_.CalledOnValidThread());
   return false;
 }
