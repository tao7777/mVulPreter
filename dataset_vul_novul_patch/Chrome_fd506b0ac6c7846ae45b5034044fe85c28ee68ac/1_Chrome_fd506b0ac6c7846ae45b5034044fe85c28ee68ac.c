void DocumentLoader::DetachFromFrame() {
  DCHECK(frame_);
   fetcher_->StopFetching();
   if (frame_ && !SentDidFinishLoad())
     LoadFailed(ResourceError::CancelledError(Url()));
   fetcher_->ClearContext();
 
  if (!frame_)
    return;

  application_cache_host_->DetachFromDocumentLoader();
  application_cache_host_.Clear();
  service_worker_network_provider_ = nullptr;
  WeakIdentifierMap<DocumentLoader>::NotifyObjectDestroyed(this);
  ClearResource();
  frame_ = nullptr;
}
