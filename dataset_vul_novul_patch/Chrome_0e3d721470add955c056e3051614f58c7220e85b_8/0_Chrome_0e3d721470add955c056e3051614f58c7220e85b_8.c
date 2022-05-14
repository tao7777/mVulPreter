 bool SynchronousCompositorImpl::IsExternalFlingActive() const {
   DCHECK(CalledOnValidThread());
   DCHECK(compositor_client_);
  if (!registered_with_client_)
    return false;
   return compositor_client_->IsExternalFlingActive();
 }
