 gfx::Rect ShellWindowFrameView::GetBoundsForClientView() const {
  if (is_frameless_ || frame_->IsFullscreen())
     return bounds();
   return gfx::Rect(0, kCaptionHeight, width(),
       std::max(0, height() - kCaptionHeight));
 }
