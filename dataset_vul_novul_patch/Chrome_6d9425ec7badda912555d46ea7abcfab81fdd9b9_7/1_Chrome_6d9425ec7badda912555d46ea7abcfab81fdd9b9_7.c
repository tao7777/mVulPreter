 SynchronousCompositorOutputSurface::DemandDrawHw(
    gfx::Size surface_size,
     const gfx::Transform& transform,
    gfx::Rect viewport,
    gfx::Rect clip,
    gfx::Rect viewport_rect_for_tile_priority,
     const gfx::Transform& transform_for_tile_priority) {
   DCHECK(CalledOnValidThread());
   DCHECK(HasClient());
  DCHECK(context_provider_.get());

  surface_size_ = surface_size;
  InvokeComposite(transform, viewport, clip, viewport_rect_for_tile_priority,
                  transform_for_tile_priority, true);

  return frame_holder_.Pass();
}
