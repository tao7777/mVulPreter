void BrowserViewRenderer::ScrollTo(gfx::Vector2d scroll_offset) {
   gfx::Vector2d max_offset = max_scroll_offset();
   gfx::Vector2dF scroll_offset_dip;
  if (max_offset.x()) {
    scroll_offset_dip.set_x((scroll_offset.x() * max_scroll_offset_dip_.x()) /
                            max_offset.x());
  }
  if (max_offset.y()) {
    scroll_offset_dip.set_y((scroll_offset.y() * max_scroll_offset_dip_.y()) /
                            max_offset.y());
  }

  DCHECK_LE(0.f, scroll_offset_dip.x());
  DCHECK_LE(0.f, scroll_offset_dip.y());
  DCHECK(scroll_offset_dip.x() < max_scroll_offset_dip_.x() ||
         scroll_offset_dip.x() - max_scroll_offset_dip_.x() < kEpsilon)
      << scroll_offset_dip.x() << " " << max_scroll_offset_dip_.x();
  DCHECK(scroll_offset_dip.y() < max_scroll_offset_dip_.y() ||
         scroll_offset_dip.y() - max_scroll_offset_dip_.y() < kEpsilon)
      << scroll_offset_dip.y() << " " << max_scroll_offset_dip_.y();

  if (scroll_offset_dip_ == scroll_offset_dip)
    return;

  scroll_offset_dip_ = scroll_offset_dip;

  TRACE_EVENT_INSTANT2("android_webview",
               "BrowserViewRenderer::ScrollTo",
               TRACE_EVENT_SCOPE_THREAD,
               "x",
               scroll_offset_dip.x(),
               "y",
               scroll_offset_dip.y());

  if (compositor_) {
    compositor_->DidChangeRootLayerScrollOffset(
        gfx::ScrollOffset(scroll_offset_dip_));
  }
}
