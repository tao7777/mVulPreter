void FragmentPaintPropertyTreeBuilder::UpdateTransform() {
  if (object_.IsSVGChild()) {
    UpdateTransformForNonRootSVG();
    return;
  }

  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    const ComputedStyle& style = object_.StyleRef();
    if (NeedsTransform(object_)) {
      TransformPaintPropertyNode::State state;

      if (object_.IsBox()) {
        auto& box = ToLayoutBox(object_);
        state.origin = TransformOrigin(box);
        style.ApplyTransform(
            state.matrix, box.Size(), ComputedStyle::kExcludeTransformOrigin,
            ComputedStyle::kIncludeMotionPath,
            ComputedStyle::kIncludeIndependentTransformProperties);

        if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
            RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
          state.rendering_context_id = context_.current.rendering_context_id;
          if (style.Preserves3D() && !state.rendering_context_id) {
            state.rendering_context_id =
                PtrHash<const LayoutObject>::GetHash(&object_);
          }
          state.direct_compositing_reasons =
              CompositingReasonsForTransform(box);
        }
      }

      state.flattens_inherited_transform =
          context_.current.should_flatten_inherited_transform;

      if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
          RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
        state.backface_visibility =
            object_.HasHiddenBackface()
                ? TransformPaintPropertyNode::BackfaceVisibility::kHidden
                : TransformPaintPropertyNode::BackfaceVisibility::kVisible;
        state.compositor_element_id = CompositorElementIdFromUniqueObjectId(
             object_.UniqueId(), CompositorElementIdNamespace::kPrimary);
       }
 
      OnUpdate(properties_->UpdateTransform(context_.current.transform,
                                             std::move(state)));
     } else {
       OnClear(properties_->ClearTransform());
    }
  }

  if (properties_->Transform()) {
    context_.current.transform = properties_->Transform();
    if (object_.StyleRef().Preserves3D()) {
      context_.current.rendering_context_id =
          properties_->Transform()->RenderingContextId();
      context_.current.should_flatten_inherited_transform = false;
    } else {
      context_.current.rendering_context_id = 0;
      context_.current.should_flatten_inherited_transform = true;
    }
  }
}
