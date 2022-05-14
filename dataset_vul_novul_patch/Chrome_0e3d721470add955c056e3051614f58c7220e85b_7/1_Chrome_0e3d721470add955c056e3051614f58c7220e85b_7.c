 gfx::ScrollOffset SynchronousCompositorImpl::GetTotalScrollOffset() {
   DCHECK(CalledOnValidThread());
   DCHECK(compositor_client_);
   return gfx::ScrollOffset(
      compositor_client_->GetTotalRootLayerScrollOffset());
}
