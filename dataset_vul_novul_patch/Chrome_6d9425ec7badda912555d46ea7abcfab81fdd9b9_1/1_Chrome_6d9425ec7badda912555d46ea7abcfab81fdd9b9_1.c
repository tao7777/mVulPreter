 void BrowserViewRenderer::SetTotalRootLayerScrollOffset(
    gfx::Vector2dF scroll_offset_dip) {
   if (scroll_offset_dip_ == scroll_offset_dip)
     return;
   scroll_offset_dip_ = scroll_offset_dip;

  gfx::Vector2d max_offset = max_scroll_offset();
  gfx::Vector2d scroll_offset;
  if (max_scroll_offset_dip_.x()) {
    scroll_offset.set_x((scroll_offset_dip.x() * max_offset.x()) /
                        max_scroll_offset_dip_.x());
  }

  if (max_scroll_offset_dip_.y()) {
    scroll_offset.set_y((scroll_offset_dip.y() * max_offset.y()) /
                        max_scroll_offset_dip_.y());
  }

  DCHECK_LE(0, scroll_offset.x());
  DCHECK_LE(0, scroll_offset.y());
  DCHECK_LE(scroll_offset.x(), max_offset.x());
  DCHECK_LE(scroll_offset.y(), max_offset.y());

  client_->ScrollContainerViewTo(scroll_offset);
}
