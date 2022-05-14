 void SynchronousCompositorOutputSurface::InvokeComposite(
     const gfx::Transform& transform,
    gfx::Rect viewport,
    gfx::Rect clip,
    gfx::Rect viewport_rect_for_tile_priority,
    gfx::Transform transform_for_tile_priority,
     bool hardware_draw) {
   DCHECK(!frame_holder_.get());
 
  gfx::Transform adjusted_transform = transform;
  adjusted_transform.matrix().postTranslate(-viewport.x(), -viewport.y(), 0);
  SetExternalDrawConstraints(adjusted_transform, viewport, clip,
                             viewport_rect_for_tile_priority,
                             transform_for_tile_priority, !hardware_draw);
  if (!hardware_draw || next_hardware_draw_needs_damage_) {
    next_hardware_draw_needs_damage_ = false;
    SetNeedsRedrawRect(gfx::Rect(viewport.size()));
  }

  client_->OnDraw();

  if (hardware_draw) {
    cached_hw_transform_ = adjusted_transform;
    cached_hw_viewport_ = viewport;
    cached_hw_clip_ = clip;
    cached_hw_viewport_rect_for_tile_priority_ =
        viewport_rect_for_tile_priority;
    cached_hw_transform_for_tile_priority_ = transform_for_tile_priority;
  } else {
    bool resourceless_software_draw = false;
    SetExternalDrawConstraints(cached_hw_transform_,
                               cached_hw_viewport_,
                               cached_hw_clip_,
                               cached_hw_viewport_rect_for_tile_priority_,
                               cached_hw_transform_for_tile_priority_,
                               resourceless_software_draw);
    next_hardware_draw_needs_damage_ = true;
  }

  if (frame_holder_.get())
    client_->DidSwapBuffersComplete();
}
