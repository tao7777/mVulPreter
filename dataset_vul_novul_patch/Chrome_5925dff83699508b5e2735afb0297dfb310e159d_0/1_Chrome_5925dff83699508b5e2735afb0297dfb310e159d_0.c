void Browser::WindowFullscreenStateChanged() {
  UpdateCommandsForFullscreenMode(window_->IsFullscreen());
  UpdateBookmarkBarState(BOOKMARK_BAR_STATE_CHANGE_TOGGLE_FULLSCREEN);
   MessageLoop::current()->PostTask(
       FROM_HERE, method_factory_.NewRunnableMethod(
       &Browser::NotifyFullscreenChange));
  if (!window_->IsFullscreen())
     NotifyTabOfFullscreenExitIfNecessary();
 }
