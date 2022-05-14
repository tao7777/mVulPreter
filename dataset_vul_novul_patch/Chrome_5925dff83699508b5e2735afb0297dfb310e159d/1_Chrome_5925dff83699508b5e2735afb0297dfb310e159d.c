void Browser::ToggleFullscreenModeForTab(TabContents* tab,
    bool enter_fullscreen) {
  if (tab != GetSelectedTabContents())
     return;
   fullscreened_tab_ = enter_fullscreen ?
       TabContentsWrapper::GetCurrentWrapperForContents(tab) : NULL;
  if (enter_fullscreen && !window_->IsFullscreen())
     tab_caused_fullscreen_ = true;
  if (tab_caused_fullscreen_)
     ToggleFullscreenMode();
 }
