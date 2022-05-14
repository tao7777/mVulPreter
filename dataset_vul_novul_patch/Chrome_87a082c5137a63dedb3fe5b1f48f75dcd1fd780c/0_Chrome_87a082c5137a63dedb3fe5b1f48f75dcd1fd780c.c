 void Layer::SetScrollOffset(gfx::Vector2d scroll_offset) {
   DCHECK(IsPropertyChangeAllowed());
 
   if (scroll_offset_ == scroll_offset)
     return;
   scroll_offset_ = scroll_offset;
  SetNeedsCommit();
}
