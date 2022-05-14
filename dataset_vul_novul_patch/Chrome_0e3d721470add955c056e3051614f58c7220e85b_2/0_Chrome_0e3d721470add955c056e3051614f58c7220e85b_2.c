 void SynchronousCompositorImpl::DidActivatePendingTree() {
   DCHECK(compositor_client_);
  if (registered_with_client_)
    compositor_client_->DidUpdateContent();
   DeliverMessages();
 }
