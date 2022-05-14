void FragmentPaintPropertyTreeBuilder::UpdateOverflowControlsClip() {
  DCHECK(properties_);

  if (!NeedsPaintPropertyUpdate())
    return;

  if (NeedsOverflowControlsClip()) {
     properties_->UpdateOverflowControlsClip(
        *context_.current.clip,
         ClipPaintPropertyNode::State{
             context_.current.transform,
             ToClipRect(LayoutRect(context_.current.paint_offset,
                                  ToLayoutBox(object_).Size()))});
  } else {
    properties_->ClearOverflowControlsClip();
  }

}
