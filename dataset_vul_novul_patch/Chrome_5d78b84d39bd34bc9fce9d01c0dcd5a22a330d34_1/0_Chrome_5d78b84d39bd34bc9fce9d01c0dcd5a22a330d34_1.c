void LayerTreeHost::PushPropertiesTo(LayerTreeImpl* tree_impl) {
  tree_impl->set_needs_full_tree_sync(needs_full_tree_sync_);
  needs_full_tree_sync_ = false;

  if (hud_layer_.get()) {
    LayerImpl* hud_impl = tree_impl->LayerById(hud_layer_->id());
    tree_impl->set_hud_layer(static_cast<HeadsUpDisplayLayerImpl*>(hud_impl));
  } else {
    tree_impl->set_hud_layer(nullptr);
  }

  tree_impl->set_background_color(background_color_);
  tree_impl->set_has_transparent_background(has_transparent_background_);
  tree_impl->set_have_scroll_event_handlers(have_scroll_event_handlers_);
  tree_impl->set_event_listener_properties(
      EventListenerClass::kTouchStartOrMove,
      event_listener_properties(EventListenerClass::kTouchStartOrMove));
  tree_impl->set_event_listener_properties(
      EventListenerClass::kMouseWheel,
      event_listener_properties(EventListenerClass::kMouseWheel));
  tree_impl->set_event_listener_properties(
      EventListenerClass::kTouchEndOrCancel,
      event_listener_properties(EventListenerClass::kTouchEndOrCancel));

  if (page_scale_layer_ && inner_viewport_scroll_layer_) {
    tree_impl->SetViewportLayersFromIds(
        overscroll_elasticity_layer_ ? overscroll_elasticity_layer_->id()
                                     : Layer::INVALID_ID,
        page_scale_layer_->id(), inner_viewport_scroll_layer_->id(),
        outer_viewport_scroll_layer_ ? outer_viewport_scroll_layer_->id()
                                     : Layer::INVALID_ID);
    DCHECK(inner_viewport_scroll_layer_->IsContainerForFixedPositionLayers());
  } else {
    tree_impl->ClearViewportLayers();
  }

  tree_impl->RegisterSelection(selection_);

  bool property_trees_changed_on_active_tree =
      tree_impl->IsActiveTree() && tree_impl->property_trees()->changed;
  if (root_layer_ && property_trees_changed_on_active_tree) {
    if (property_trees_.sequence_number ==
        tree_impl->property_trees()->sequence_number)
      tree_impl->property_trees()->PushChangeTrackingTo(&property_trees_);
    else
      tree_impl->MoveChangeTrackingToLayers();
  }
  tree_impl->SetPropertyTrees(&property_trees_);

  tree_impl->PushPageScaleFromMainThread(
      page_scale_factor_, min_page_scale_factor_, max_page_scale_factor_);

  tree_impl->set_browser_controls_shrink_blink_size(
      browser_controls_shrink_blink_size_);
  tree_impl->set_top_controls_height(top_controls_height_);
  tree_impl->set_bottom_controls_height(bottom_controls_height_);
  tree_impl->PushBrowserControlsFromMainThread(top_controls_shown_ratio_);
  tree_impl->elastic_overscroll()->PushFromMainThread(elastic_overscroll_);
  if (tree_impl->IsActiveTree())
    tree_impl->elastic_overscroll()->PushPendingToActive();

  tree_impl->set_painted_device_scale_factor(painted_device_scale_factor_);
 
   tree_impl->SetDeviceColorSpace(device_color_space_);
 
  tree_impl->set_content_source_id(content_source_id_);

   if (pending_page_scale_animation_) {
     tree_impl->SetPendingPageScaleAnimation(
         std::move(pending_page_scale_animation_));
  }

  DCHECK(!tree_impl->ViewportSizeInvalid());

  tree_impl->set_has_ever_been_drawn(false);
}
