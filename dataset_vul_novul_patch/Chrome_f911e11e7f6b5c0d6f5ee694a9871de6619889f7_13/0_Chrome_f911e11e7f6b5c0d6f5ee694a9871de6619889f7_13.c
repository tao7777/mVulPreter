void FragmentPaintPropertyTreeBuilder::UpdateScrollAndScrollTranslation() {
  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsScrollNode(object_)) {
      const LayoutBox& box = ToLayoutBox(object_);
      auto* scrollable_area = box.GetScrollableArea();
      ScrollPaintPropertyNode::State state;

      state.container_rect = PixelSnappedIntRect(
          box.OverflowClipRect(context_.current.paint_offset));
      state.contents_rect = IntRect(
          -scrollable_area->ScrollOrigin() + state.container_rect.Location(),
          scrollable_area->ContentsSize());
      if (box.HasFlippedBlocksWritingMode())
        state.contents_rect.Move(box.VerticalScrollbarWidth(), 0);

      state.user_scrollable_horizontal =
          scrollable_area->UserInputScrollable(kHorizontalScrollbar);
      state.user_scrollable_vertical =
          scrollable_area->UserInputScrollable(kVerticalScrollbar);

      auto ancestor_reasons =
          context_.current.scroll->GetMainThreadScrollingReasons();
      state.main_thread_scrolling_reasons =
          GetMainThreadScrollingReasons(object_, ancestor_reasons);

      if (auto* existing_scroll = properties_->Scroll()) {
        if (existing_scroll->GetMainThreadScrollingReasons() !=
            state.main_thread_scrolling_reasons)
          full_context_.force_subtree_update = true;
      }

      if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
           RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled())
         state.compositor_element_id = scrollable_area->GetCompositorElementId();
 
      OnUpdate(properties_->UpdateScroll(*context_.current.scroll,
                                         std::move(state)));
     } else {
       OnClear(properties_->ClearScroll());
     }

    if (NeedsScrollOrScrollTranslation(object_)) {
      const LayoutBox& box = ToLayoutBox(object_);
      TransformPaintPropertyNode::State state;
      IntSize scroll_offset = box.ScrolledContentOffset();
      state.matrix.Translate(-scroll_offset.Width(), -scroll_offset.Height());
      state.flattens_inherited_transform =
          context_.current.should_flatten_inherited_transform;
      if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
          RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
        state.direct_compositing_reasons = CompositingReasonsForScroll(box);
         state.rendering_context_id = context_.current.rendering_context_id;
       }
       state.scroll = properties_->Scroll();
      OnUpdate(properties_->UpdateScrollTranslation(*context_.current.transform,
                                                     std::move(state)));
     } else {
       OnClear(properties_->ClearScrollTranslation());
    }
  }

  if (properties_->Scroll())
    context_.current.scroll = properties_->Scroll();
  if (properties_->ScrollTranslation()) {
    context_.current.transform = properties_->ScrollTranslation();
    context_.current.should_flatten_inherited_transform = false;
  }
}
