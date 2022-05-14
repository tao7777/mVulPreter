void FragmentPaintPropertyTreeBuilder::UpdateOverflowClip() {
  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsOverflowClip(object_) && !CanOmitOverflowClip(object_)) {
      ClipPaintPropertyNode::State state;
      state.local_transform_space = context_.current.transform;

      if (!RuntimeEnabledFeatures::SlimmingPaintV175Enabled() &&
          object_.IsSVGForeignObject()) {
        state.clip_rect = ToClipRect(ToLayoutBox(object_).FrameRect());
      } else if (object_.IsBox()) {
        state.clip_rect = ToClipRect(ToLayoutBox(object_).OverflowClipRect(
            context_.current.paint_offset));
        state.clip_rect_excluding_overlay_scrollbars =
            ToClipRect(ToLayoutBox(object_).OverflowClipRect(
                context_.current.paint_offset,
                kExcludeOverlayScrollbarSizeForHitTesting));
      } else {
        DCHECK(object_.IsSVGViewportContainer());
        const auto& viewport_container = ToLayoutSVGViewportContainer(object_);
        state.clip_rect = FloatRoundedRect(
            viewport_container.LocalToSVGParentTransform().Inverse().MapRect(
                viewport_container.Viewport()));
      }

      const ClipPaintPropertyNode* existing = properties_->OverflowClip();
       bool equal_ignoring_hit_test_rects =
           !!existing &&
           existing->EqualIgnoringHitTestRects(context_.current.clip, state);
      OnUpdateClip(properties_->UpdateOverflowClip(*context_.current.clip,
                                                    std::move(state)),
                    equal_ignoring_hit_test_rects);
     } else {
      OnClearClip(properties_->ClearOverflowClip());
    }
  }

  if (auto* overflow_clip = properties_->OverflowClip())
    context_.current.clip = overflow_clip;
}
