void FragmentPaintPropertyTreeBuilder::UpdateOutOfFlowContext() {
  if (!object_.IsBoxModelObject() && !properties_)
    return;

  if (object_.IsLayoutBlock())
    context_.paint_offset_for_float = context_.current.paint_offset;

  if (object_.CanContainAbsolutePositionObjects()) {
    context_.absolute_position = context_.current;
  }

  if (object_.IsLayoutView()) {
    const auto* initial_fixed_transform = context_.fixed_position.transform;
    const auto* initial_fixed_scroll = context_.fixed_position.scroll;

    context_.fixed_position = context_.current;
    context_.fixed_position.fixed_position_children_fixed_to_root = true;

    context_.fixed_position.transform = initial_fixed_transform;
    context_.fixed_position.scroll = initial_fixed_scroll;
  } else if (object_.CanContainFixedPositionObjects()) {
    context_.fixed_position = context_.current;
    context_.fixed_position.fixed_position_children_fixed_to_root = false;
  } else if (properties_ && properties_->CssClip()) {
    auto* css_clip = properties_->CssClip();

    if (context_.fixed_position.clip == css_clip->Parent()) {
      context_.fixed_position.clip = css_clip;
     } else {
       if (NeedsPaintPropertyUpdate()) {
         OnUpdate(properties_->UpdateCssClipFixedPosition(
            *context_.fixed_position.clip,
             ClipPaintPropertyNode::State{css_clip->LocalTransformSpace(),
                                          css_clip->ClipRect()}));
       }
      if (properties_->CssClipFixedPosition())
        context_.fixed_position.clip = properties_->CssClipFixedPosition();
      return;
    }
  }

  if (NeedsPaintPropertyUpdate() && properties_)
    OnClear(properties_->ClearCssClipFixedPosition());
}
