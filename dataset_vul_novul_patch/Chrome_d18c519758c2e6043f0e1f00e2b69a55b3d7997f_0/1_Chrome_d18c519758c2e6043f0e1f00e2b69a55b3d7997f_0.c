 void WebContentsImpl::DidCallFocus() {
  if (IsFullscreenForCurrentTab())
    ExitFullscreen(true);
 }
