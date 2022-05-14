ShelfBackgroundType ShelfLayoutManager::GetShelfBackgroundType() const {
  if (state_.pre_lock_screen_animation_active)
    return SHELF_BACKGROUND_DEFAULT;

  if (state_.session_state == session_manager::SessionState::OOBE)
    return SHELF_BACKGROUND_OOBE;
  if (state_.session_state != session_manager::SessionState::ACTIVE) {
    if (Shell::Get()->wallpaper_controller()->HasShownAnyWallpaper() &&
        !Shell::Get()->wallpaper_controller()->IsWallpaperBlurred()) {
      return SHELF_BACKGROUND_LOGIN_NONBLURRED_WALLPAPER;
    }
    return SHELF_BACKGROUND_LOGIN;
  }

  if (is_app_list_visible_) {
    if (!IsHomeLauncherEnabledInTabletMode())
      return SHELF_BACKGROUND_APP_LIST;

    if (is_home_launcher_shown_ || is_home_launcher_target_position_shown_)
       return SHELF_BACKGROUND_DEFAULT;
   }
 
  if (Shell::Get()->IsSplitViewModeActive() ||
      (state_.visibility_state != SHELF_AUTO_HIDE &&
       state_.window_state == wm::WORKSPACE_WINDOW_STATE_MAXIMIZED)) {
     return SHELF_BACKGROUND_MAXIMIZED;
   }
 
   if (Shell::Get()->overview_controller() &&
       Shell::Get()->overview_controller()->IsSelecting()) {
     return SHELF_BACKGROUND_OVERVIEW;
  }

  return SHELF_BACKGROUND_DEFAULT;
}
