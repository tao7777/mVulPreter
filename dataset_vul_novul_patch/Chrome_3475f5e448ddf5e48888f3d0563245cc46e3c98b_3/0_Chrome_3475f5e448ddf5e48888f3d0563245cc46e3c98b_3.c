 bool LauncherView::IsShowingMenu() const {
 #if !defined(OS_MACOSX)
  return (launcher_menu_runner_.get() &&
        launcher_menu_runner_->IsRunning());
 #endif
   return false;
 }
