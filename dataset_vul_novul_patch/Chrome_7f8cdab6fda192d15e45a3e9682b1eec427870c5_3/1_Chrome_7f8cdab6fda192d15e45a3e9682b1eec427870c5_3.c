 gfx::Size ShellWindowFrameView::GetMinimumSize() {
   gfx::Size min_size = frame_->client_view()->GetMinimumSize();
   gfx::Rect client_bounds = GetBoundsForClientView();
   min_size.Enlarge(0, client_bounds.y());
  int closeButtonOffsetX =
      (kCaptionHeight - close_button_->height()) / 2;
  int header_width = close_button_->width() + closeButtonOffsetX * 2;
  if (header_width > min_size.width())
    min_size.set_width(header_width);
  return min_size;
}
