void RenderWidgetHostViewAura::AcceleratedSurfaceRelease(
void RenderWidgetHostViewAura::AcceleratedSurfaceRelease() {
  // This really tells us to release the frontbuffer.
  if (current_surface_ && ShouldReleaseFrontSurface()) {
    ui::Compositor* compositor = GetCompositor();
    if (compositor) {
      // We need to wait for a commit to clear to guarantee that all we
      // will not issue any more GL referencing the previous surface.
      can_lock_compositor_ = NO_PENDING_COMMIT;
      scoped_refptr<ui::Texture> surface_ref =
          image_transport_clients_[current_surface_];
      on_compositing_did_commit_callbacks_.push_back(
          base::Bind(&RenderWidgetHostViewAura::
                     SetSurfaceNotInUseByCompositor,
                     AsWeakPtr(),
                     surface_ref));
      if (!compositor->HasObserver(this))
        compositor->AddObserver(this);
    }
    image_transport_clients_.erase(current_surface_);
     current_surface_ = 0;
     UpdateExternalTexture();
   }
 }
