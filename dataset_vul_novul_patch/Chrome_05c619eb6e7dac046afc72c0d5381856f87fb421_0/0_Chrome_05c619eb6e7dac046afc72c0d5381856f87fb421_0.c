void ShellSurface::OnSurfaceCommit() {
  surface_->CheckIfSurfaceHierarchyNeedsCommitToNewSurfaces();
  surface_->CommitSurfaceHierarchy();

  if (enabled() && !widget_)
    CreateShellSurfaceWidget(ui::SHOW_STATE_NORMAL);

  origin_ += pending_origin_offset_;
  pending_origin_offset_ = gfx::Vector2d();

  resize_component_ = pending_resize_component_;

  if (widget_) {
    geometry_ = pending_geometry_;

     UpdateWidgetBounds();
 
     gfx::Point surface_origin = GetSurfaceOrigin();

    // System modal container is used by clients to implement client side
    // managed system modal dialogs using a single ShellSurface instance.
    // Hit-test region will be non-empty when at least one dialog exists on
    // the client side. Here we detect the transition between no client side
    // dialog and at least one dialog so activatable state is properly
    // updated.
    if (container_ == ash::kShellWindowId_SystemModalContainer) {
      gfx::Rect hit_test_bounds =
          surface_->GetHitTestBounds() + surface_origin.OffsetFromOrigin();

      // Prevent window from being activated when hit test bounds are empty.
      bool activatable = activatable_ && !hit_test_bounds.IsEmpty();
      if (activatable != CanActivate()) {
        set_can_activate(activatable);

        // Activate or deactivate window if activation state changed.
        aura::client::ActivationClient* activation_client =
            ash::Shell::GetInstance()->activation_client();
        if (activatable)
          activation_client->ActivateWindow(widget_->GetNativeWindow());
        else if (widget_->IsActive())
          activation_client->DeactivateWindow(widget_->GetNativeWindow());
      }
     }
 
    surface_->window()->SetBounds(
        gfx::Rect(surface_origin, surface_->window()->layer()->size()));

    if (pending_scale_ != scale_) {
      gfx::Transform transform;
      DCHECK_NE(pending_scale_, 0.0);
      transform.Scale(1.0 / pending_scale_, 1.0 / pending_scale_);
      surface_->window()->SetTransform(transform);
      scale_ = pending_scale_;
    }

    if (pending_show_widget_) {
      DCHECK(!widget_->IsClosed());
      DCHECK(!widget_->IsVisible());
      pending_show_widget_ = false;
      widget_->Show();
    }
  }
}
