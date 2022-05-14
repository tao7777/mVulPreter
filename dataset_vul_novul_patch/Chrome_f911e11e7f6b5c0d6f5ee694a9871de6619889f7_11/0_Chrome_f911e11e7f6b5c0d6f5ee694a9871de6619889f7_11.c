void FragmentPaintPropertyTreeBuilder::UpdatePaintOffsetTranslation(
    const base::Optional<IntPoint>& paint_offset_translation) {
  DCHECK(properties_);

  if (paint_offset_translation) {
    TransformPaintPropertyNode::State state;
    state.matrix.Translate(paint_offset_translation->X(),
                           paint_offset_translation->Y());
    state.flattens_inherited_transform =
        context_.current.should_flatten_inherited_transform;
    if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
         RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled())
       state.rendering_context_id = context_.current.rendering_context_id;
     OnUpdate(properties_->UpdatePaintOffsetTranslation(
        *context_.current.transform, std::move(state)));
     context_.current.transform = properties_->PaintOffsetTranslation();
     if (object_.IsLayoutView()) {
       context_.absolute_position.transform =
          properties_->PaintOffsetTranslation();
      context_.fixed_position.transform = properties_->PaintOffsetTranslation();
    }
  } else {
    OnClear(properties_->ClearPaintOffsetTranslation());
  }
}
