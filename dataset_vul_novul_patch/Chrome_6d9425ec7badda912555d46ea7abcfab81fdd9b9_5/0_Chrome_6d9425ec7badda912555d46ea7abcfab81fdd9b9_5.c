 scoped_ptr<cc::CompositorFrame> SynchronousCompositorImpl::DemandDrawHw(
    const gfx::Size& surface_size,
     const gfx::Transform& transform,
    const gfx::Rect& viewport,
    const gfx::Rect& clip,
    const gfx::Rect& viewport_rect_for_tile_priority,
     const gfx::Transform& transform_for_tile_priority) {
   DCHECK(CalledOnValidThread());
   DCHECK(output_surface_);
  DCHECK(begin_frame_source_);

  scoped_ptr<cc::CompositorFrame> frame =
      output_surface_->DemandDrawHw(surface_size,
                                    transform,
                                    viewport,
                                    clip,
                                    viewport_rect_for_tile_priority,
                                    transform_for_tile_priority);

  if (frame.get())
    UpdateFrameMetaData(frame->metadata);

  return frame.Pass();
}
