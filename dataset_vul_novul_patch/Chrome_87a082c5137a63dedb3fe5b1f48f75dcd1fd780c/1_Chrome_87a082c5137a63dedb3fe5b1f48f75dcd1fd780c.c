 void Layer::SetScrollOffset(gfx::Vector2d scroll_offset) {
   DCHECK(IsPropertyChangeAllowed());
 
  if (layer_tree_host()) {
    scroll_offset = layer_tree_host()->DistributeScrollOffsetToViewports(
        scroll_offset, this);
  }
   if (scroll_offset_ == scroll_offset)
     return;
   scroll_offset_ = scroll_offset;
  SetNeedsCommit();
}
