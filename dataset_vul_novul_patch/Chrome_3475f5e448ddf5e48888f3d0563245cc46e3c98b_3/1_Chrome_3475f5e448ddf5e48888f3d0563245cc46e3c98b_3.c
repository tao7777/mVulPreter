 bool LauncherView::IsShowingMenu() const {
 #if !defined(OS_MACOSX)
  return (overflow_menu_runner_.get() &&
          overflow_menu_runner_->IsRunning()) ||
      (launcher_menu_runner_.get() &&
        launcher_menu_runner_->IsRunning());
 #endif
   return false;
 }
