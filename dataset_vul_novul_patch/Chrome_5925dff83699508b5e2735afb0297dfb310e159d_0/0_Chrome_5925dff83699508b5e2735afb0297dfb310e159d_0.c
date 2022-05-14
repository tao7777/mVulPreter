void Browser::WindowFullscreenStateChanged() {
  UpdateCommandsForFullscreenMode(window_->IsFullscreen());
  UpdateBookmarkBarState(BOOKMARK_BAR_STATE_CHANGE_TOGGLE_FULLSCREEN);
   MessageLoop::current()->PostTask(
       FROM_HERE, method_factory_.NewRunnableMethod(
       &Browser::NotifyFullscreenChange));
  bool notify_tab_of_exit;
#if defined(OS_MACOSX)
  notify_tab_of_exit = !window_->InPresentationMode();
#else
  notify_tab_of_exit = !window_->IsFullscreen();
#endif
  if (notify_tab_of_exit)
     NotifyTabOfFullscreenExitIfNecessary();
 }
