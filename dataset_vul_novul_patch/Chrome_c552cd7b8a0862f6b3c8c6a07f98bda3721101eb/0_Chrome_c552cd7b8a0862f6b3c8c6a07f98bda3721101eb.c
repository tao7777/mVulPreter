void Browser::AddNewContents(WebContents* source,
                             std::unique_ptr<WebContents> new_contents,
                             WindowOpenDisposition disposition,
                              const gfx::Rect& initial_rect,
                              bool user_gesture,
                              bool* was_blocked) {
#if defined(OS_MACOSX)
  // On the Mac, the convention is to turn popups into new tabs when in
  // fullscreen mode. Only worry about user-initiated fullscreen as showing a
  // popup in HTML5 fullscreen would have kicked the page out of fullscreen.
  if (disposition == WindowOpenDisposition::NEW_POPUP &&
      exclusive_access_manager_->fullscreen_controller()
          ->IsFullscreenForBrowser()) {
    disposition = WindowOpenDisposition::NEW_FOREGROUND_TAB;
  }
#endif

   if (source && PopupBlockerTabHelper::ConsiderForPopupBlocking(disposition))
    PopupTracker::CreateForWebContents(new_contents.get(), source);
  chrome::AddWebContents(this, source, std::move(new_contents), disposition,
                         initial_rect);
}
