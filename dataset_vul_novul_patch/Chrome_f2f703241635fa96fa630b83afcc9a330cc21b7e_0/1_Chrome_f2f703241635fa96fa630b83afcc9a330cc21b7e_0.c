int ShelfBackgroundAnimator::GetBackgroundAlphaValue(
    ShelfBackgroundType background_type) const {
  switch (background_type) {
    case SHELF_BACKGROUND_DEFAULT:
    case SHELF_BACKGROUND_OVERVIEW:
      return kShelfTranslucentAlpha;
    case SHELF_BACKGROUND_MAXIMIZED:
       return kShelfTranslucentMaximizedWindow;
     case SHELF_BACKGROUND_APP_LIST:
       return kShelfTranslucentOverAppList;
    case SHELF_BACKGROUND_SPLIT_VIEW:
      return ShelfBackgroundAnimator::kMaxAlpha;
     case SHELF_BACKGROUND_OOBE:
       return SK_AlphaTRANSPARENT;
     case SHELF_BACKGROUND_LOGIN:
      return SK_AlphaTRANSPARENT;
    case SHELF_BACKGROUND_LOGIN_NONBLURRED_WALLPAPER:
      return login_constants::kNonBlurredWallpaperBackgroundAlpha;
  }
  return SK_AlphaTRANSPARENT;
}
