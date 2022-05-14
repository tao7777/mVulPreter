 int BrowserNonClientFrameViewAura::NonClientTopBorderHeight(
     bool force_restored) const {
   if (frame()->widget_delegate() &&
       frame()->widget_delegate()->ShouldShowWindowTitle()) {
     return close_button_->bounds().bottom();
   }
  if (!frame()->IsMaximized() || force_restored)
    return kTabstripTopSpacingRestored;
  return kTabstripTopSpacingMaximized;
 }
