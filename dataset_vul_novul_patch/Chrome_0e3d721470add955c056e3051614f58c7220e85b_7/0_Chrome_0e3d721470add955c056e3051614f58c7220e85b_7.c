 gfx::ScrollOffset SynchronousCompositorImpl::GetTotalScrollOffset() {
   DCHECK(CalledOnValidThread());
   DCHECK(compositor_client_);
  if (!registered_with_client_)
    return gfx::ScrollOffset();
   return gfx::ScrollOffset(
      compositor_client_->GetTotalRootLayerScrollOffset());
}
