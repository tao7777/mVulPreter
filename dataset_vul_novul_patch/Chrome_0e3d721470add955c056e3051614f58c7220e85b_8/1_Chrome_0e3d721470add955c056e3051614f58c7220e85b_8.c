 bool SynchronousCompositorImpl::IsExternalFlingActive() const {
   DCHECK(CalledOnValidThread());
   DCHECK(compositor_client_);
   return compositor_client_->IsExternalFlingActive();
 }
