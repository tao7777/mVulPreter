 void ShellWindowFrameView::Layout() {
   gfx::Size close_size = close_button_->GetPreferredSize();
   int closeButtonOffsetY =
       (kCaptionHeight - close_size.height()) / 2;
  int closeButtonOffsetX = closeButtonOffsetY;
  close_button_->SetBounds(
      width() - closeButtonOffsetX - close_size.width(),
      closeButtonOffsetY,
      close_size.width(),
      close_size.height());
 }
