void SynchronousCompositorImpl::UpdateRootLayerState(
    const gfx::ScrollOffset& total_scroll_offset,
    const gfx::ScrollOffset& max_scroll_offset,
    const gfx::SizeF& scrollable_size,
    float page_scale_factor,
    float min_page_scale_factor,
    float max_page_scale_factor) {
   DCHECK(CalledOnValidThread());
   DCHECK(compositor_client_);
 
  if (registered_with_client_) {
    // TODO(miletus): Pass in ScrollOffset. crbug.com/414283.
    compositor_client_->UpdateRootLayerState(
        gfx::ScrollOffsetToVector2dF(total_scroll_offset),
        gfx::ScrollOffsetToVector2dF(max_scroll_offset),
        scrollable_size,
        page_scale_factor,
        min_page_scale_factor,
        max_page_scale_factor);
  }
 }
