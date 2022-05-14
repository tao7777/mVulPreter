ShelfLayoutManager::AutoHideState ShelfLayoutManager::CalculateAutoHideState(
    VisibilityState visibility_state) const {
  if (visibility_state != AUTO_HIDE || !launcher_widget())
    return AUTO_HIDE_HIDDEN;

  Shell* shell = Shell::GetInstance();
  if (shell->GetAppListTargetVisibility())
    return AUTO_HIDE_SHOWN;

  if (shell->system_tray() && shell->system_tray()->should_show_launcher())
    return AUTO_HIDE_SHOWN;

   if (launcher_ && launcher_->IsShowingMenu())
     return AUTO_HIDE_SHOWN;
 
   if (launcher_widget()->IsActive() || status_->IsActive())
     return AUTO_HIDE_SHOWN;
 
  if (event_filter_.get() && event_filter_->in_mouse_drag())
    return AUTO_HIDE_HIDDEN;

  aura::RootWindow* root = launcher_widget()->GetNativeView()->GetRootWindow();
  bool mouse_over_launcher =
      launcher_widget()->GetWindowScreenBounds().Contains(
          root->last_mouse_location());
  return mouse_over_launcher ? AUTO_HIDE_SHOWN : AUTO_HIDE_HIDDEN;
}
