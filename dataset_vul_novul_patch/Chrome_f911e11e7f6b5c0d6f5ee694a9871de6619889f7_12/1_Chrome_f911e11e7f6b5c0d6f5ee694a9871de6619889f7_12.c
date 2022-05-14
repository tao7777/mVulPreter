void FragmentPaintPropertyTreeBuilder::UpdatePerspective() {
  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsPerspective(object_)) {
      const ComputedStyle& style = object_.StyleRef();
      TransformPaintPropertyNode::State state;
      state.matrix.ApplyPerspective(style.Perspective());
      state.origin = PerspectiveOrigin(ToLayoutBox(object_)) +
                     ToLayoutSize(context_.current.paint_offset);
      state.flattens_inherited_transform =
          context_.current.should_flatten_inherited_transform;
       if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
           RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled())
         state.rendering_context_id = context_.current.rendering_context_id;
      OnUpdate(properties_->UpdatePerspective(context_.current.transform,
                                               std::move(state)));
     } else {
       OnClear(properties_->ClearPerspective());
    }
  }

  if (properties_->Perspective()) {
    context_.current.transform = properties_->Perspective();
    context_.current.should_flatten_inherited_transform = false;
  }
}
