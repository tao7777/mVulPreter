 gfx::Size ShellWindowFrameView::GetMaximumSize() {
   gfx::Size max_size = frame_->client_view()->GetMaximumSize();
  if (is_frameless_)
    return max_size;

   if (!max_size.IsEmpty()) {
     gfx::Rect client_bounds = GetBoundsForClientView();
     max_size.Enlarge(0, client_bounds.y());
  }
  return max_size;
}
