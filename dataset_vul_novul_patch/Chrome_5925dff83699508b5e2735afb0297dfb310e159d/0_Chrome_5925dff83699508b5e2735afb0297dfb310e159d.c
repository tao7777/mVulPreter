void Browser::ToggleFullscreenModeForTab(TabContents* tab,
    bool enter_fullscreen) {
  if (tab != GetSelectedTabContents())
     return;
   fullscreened_tab_ = enter_fullscreen ?
       TabContentsWrapper::GetCurrentWrapperForContents(tab) : NULL;
  bool in_correct_mode_for_tab_fullscreen;
#if defined(OS_MACOSX)
  in_correct_mode_for_tab_fullscreen = window_->InPresentationMode();
#else
  in_correct_mode_for_tab_fullscreen = window_->IsFullscreen();
#endif
  if (enter_fullscreen && !in_correct_mode_for_tab_fullscreen)
     tab_caused_fullscreen_ = true;
  if (tab_caused_fullscreen_) {
#if defined(OS_MACOSX)
    TogglePresentationMode();
#else
     ToggleFullscreenMode();
#endif
  }
 }
