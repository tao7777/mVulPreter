void FragmentPaintPropertyTreeBuilder::UpdateInnerBorderRadiusClip() {
  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsInnerBorderRadiusClip(object_)) {
      const LayoutBox& box = ToLayoutBox(object_);
      ClipPaintPropertyNode::State state;
      state.local_transform_space = context_.current.transform;
      if (box.IsLayoutReplaced()) {
        state.clip_rect = box.StyleRef().GetRoundedInnerBorderFor(
            LayoutRect(context_.current.paint_offset, box.Size()),
            LayoutRectOutsets(-(box.PaddingTop() + box.BorderTop()),
                              -(box.PaddingRight() + box.BorderRight()),
                              -(box.PaddingBottom() + box.BorderBottom()),
                              -(box.PaddingLeft() + box.BorderLeft())));
      } else {
        state.clip_rect = box.StyleRef().GetRoundedInnerBorderFor(
             LayoutRect(context_.current.paint_offset, box.Size()));
       }
       OnUpdateClip(properties_->UpdateInnerBorderRadiusClip(
          *context_.current.clip, std::move(state)));
     } else {
       OnClearClip(properties_->ClearInnerBorderRadiusClip());
     }
  }

  if (auto* border_radius_clip = properties_->InnerBorderRadiusClip())
    context_.current.clip = border_radius_clip;
}
