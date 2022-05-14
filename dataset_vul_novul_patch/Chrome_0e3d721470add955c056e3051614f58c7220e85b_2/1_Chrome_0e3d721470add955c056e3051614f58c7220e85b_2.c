 void SynchronousCompositorImpl::DidActivatePendingTree() {
   DCHECK(compositor_client_);
  compositor_client_->DidUpdateContent();
   DeliverMessages();
 }
