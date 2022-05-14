CompositorFrameMetadata LayerTreeHostImpl::MakeCompositorFrameMetadata() const {
  CompositorFrameMetadata metadata;
  metadata.device_scale_factor = active_tree_->painted_device_scale_factor() *
                                 active_tree_->device_scale_factor();

  metadata.page_scale_factor = active_tree_->current_page_scale_factor();
  metadata.scrollable_viewport_size = active_tree_->ScrollableViewportSize();
  metadata.root_layer_size = active_tree_->ScrollableSize();
  metadata.min_page_scale_factor = active_tree_->min_page_scale_factor();
  metadata.max_page_scale_factor = active_tree_->max_page_scale_factor();
  metadata.top_controls_height =
      browser_controls_offset_manager_->TopControlsHeight();
  metadata.top_controls_shown_ratio =
      browser_controls_offset_manager_->TopControlsShownRatio();
  metadata.bottom_controls_height =
      browser_controls_offset_manager_->BottomControlsHeight();
   metadata.bottom_controls_shown_ratio =
       browser_controls_offset_manager_->BottomControlsShownRatio();
   metadata.root_background_color = active_tree_->background_color();
 
   active_tree_->GetViewportSelection(&metadata.selection);
 
  if (OuterViewportScrollLayer()) {
    metadata.root_overflow_x_hidden =
        !OuterViewportScrollLayer()->user_scrollable_horizontal();
    metadata.root_overflow_y_hidden =
        !OuterViewportScrollLayer()->user_scrollable_vertical();
  }

  if (GetDrawMode() == DRAW_MODE_RESOURCELESS_SOFTWARE) {
    metadata.is_resourceless_software_draw_with_scroll_or_animation =
        IsActivelyScrolling() || mutator_host_->NeedsTickAnimations();
  }

  for (LayerImpl* surface_layer : active_tree_->SurfaceLayers()) {
    SurfaceLayerImpl* surface_layer_impl =
        static_cast<SurfaceLayerImpl*>(surface_layer);
    metadata.referenced_surfaces.push_back(
        surface_layer_impl->primary_surface_info().id());
    if (surface_layer_impl->fallback_surface_info().is_valid()) {
      metadata.referenced_surfaces.push_back(
          surface_layer_impl->fallback_surface_info().id());
    }
  }
  if (!InnerViewportScrollLayer())
    return metadata;

  metadata.root_overflow_x_hidden |=
      !InnerViewportScrollLayer()->user_scrollable_horizontal();
  metadata.root_overflow_y_hidden |=
      !InnerViewportScrollLayer()->user_scrollable_vertical();

  metadata.root_scroll_offset =
      gfx::ScrollOffsetToVector2dF(active_tree_->TotalScrollOffset());

  return metadata;
}
