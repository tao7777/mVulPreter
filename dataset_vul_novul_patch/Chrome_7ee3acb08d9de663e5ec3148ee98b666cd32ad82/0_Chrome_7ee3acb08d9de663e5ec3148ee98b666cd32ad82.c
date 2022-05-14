 int BrowserNonClientFrameViewAura::NonClientTopBorderHeight(
     bool force_restored) const {
  if (force_restored)
    return kTabstripTopSpacingRestored;
  if (frame()->IsFullscreen())
    return 0;
  if (frame()->IsMaximized())
    return kTabstripTopSpacingMaximized;
   if (frame()->widget_delegate() &&
       frame()->widget_delegate()->ShouldShowWindowTitle()) {
     return close_button_->bounds().bottom();
   }
  return kTabstripTopSpacingRestored;
 }
