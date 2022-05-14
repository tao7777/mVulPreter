 gfx::Rect ShellWindowFrameView::GetWindowBoundsForClientBounds(
       const gfx::Rect& client_bounds) const {
  if (is_frameless_)
    return client_bounds;

   int closeButtonOffsetX =
       (kCaptionHeight - close_button_->height()) / 2;
   int header_width = close_button_->width() + closeButtonOffsetX * 2;
  return gfx::Rect(client_bounds.x(),
                   std::max(0, client_bounds.y() - kCaptionHeight),
                   std::max(header_width, client_bounds.width()),
                   client_bounds.height() + kCaptionHeight);
}
