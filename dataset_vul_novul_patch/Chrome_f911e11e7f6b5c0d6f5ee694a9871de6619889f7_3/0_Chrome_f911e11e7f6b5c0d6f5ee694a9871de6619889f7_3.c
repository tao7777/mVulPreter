void FragmentPaintPropertyTreeBuilder::UpdateCssClip() {
  DCHECK(properties_);

  if (NeedsPaintPropertyUpdate()) {
    if (NeedsCssClip(object_)) {
       DCHECK(object_.CanContainAbsolutePositionObjects());
       OnUpdateClip(properties_->UpdateCssClip(
          *context_.current.clip,
           ClipPaintPropertyNode::State{context_.current.transform,
                                        ToClipRect(ToLayoutBox(object_).ClipRect(
                                            context_.current.paint_offset))}));
    } else {
      OnClearClip(properties_->ClearCssClip());
    }
  }

  if (properties_->CssClip())
    context_.current.clip = properties_->CssClip();
}
