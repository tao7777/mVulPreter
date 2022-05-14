void FragmentPaintPropertyTreeBuilder::UpdateFilter() {
  DCHECK(properties_);
  const ComputedStyle& style = object_.StyleRef();

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsFilter(object_)) {
      EffectPaintPropertyNode::State state;
      state.local_transform_space = context_.current.transform;
      state.paint_offset = FloatPoint(context_.current.paint_offset);

      auto* layer = ToLayoutBoxModelObject(object_).Layer();
      if (layer) {
        if (properties_->Filter())
          state.filter = properties_->Filter()->Filter();

        if (object_.IsLayoutImage() &&
            ToLayoutImage(object_).ShouldInvertColor())
          state.filter.AppendInvertFilter(1.0f);

        layer->UpdateCompositorFilterOperationsForFilter(state.filter);
        layer->ClearFilterOnEffectNodeDirty();
      } else {
        DCHECK(object_.IsLayoutImage() &&
               ToLayoutImage(object_).ShouldInvertColor());
        state.filter = CompositorFilterOperations();
        state.filter.AppendInvertFilter(1.0f);
      }

      state.output_clip = context_.current.clip;


      if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
          RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
        state.direct_compositing_reasons =
            CompositingReasonFinder::RequiresCompositingForFilterAnimation(
                style)
                ? CompositingReason::kActiveFilterAnimation
                : CompositingReason::kNone;
        DCHECK(!style.HasCurrentFilterAnimation() ||
               state.direct_compositing_reasons != CompositingReason::kNone);

        state.compositor_element_id = CompositorElementIdFromUniqueObjectId(
             object_.UniqueId(), CompositorElementIdNamespace::kEffectFilter);
       }
 
      OnUpdate(properties_->UpdateFilter(*context_.current_effect,
                                         std::move(state)));
     } else {
       OnClear(properties_->ClearFilter());
     }
  }

  if (properties_->Filter()) {
    context_.current_effect = properties_->Filter();
    const ClipPaintPropertyNode* input_clip =
        properties_->Filter()->OutputClip();
    context_.current.clip = context_.absolute_position.clip =
        context_.fixed_position.clip = input_clip;
  }
}
