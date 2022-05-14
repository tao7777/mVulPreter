void ShelfBackgroundAnimator::GetTargetValues(
    ShelfBackgroundType background_type,
    AnimationValues* shelf_background_values) const {
  auto darken_wallpaper = [&](int darkening_alpha) {
    if (!wallpaper_controller_)
      return kShelfDefaultBaseColor;
    SkColor target_color =
        wallpaper_controller_->GetProminentColor(GetShelfColorProfile());
    if (target_color == kInvalidWallpaperColor)
      return kShelfDefaultBaseColor;
    return color_utils::GetResultingPaintColor(
        SkColorSetA(kShelfDefaultBaseColor, darkening_alpha), target_color);
  };

  SkColor shelf_target_color = kShelfDefaultBaseColor;
  switch (background_type) {
    case SHELF_BACKGROUND_DEFAULT:
    case SHELF_BACKGROUND_APP_LIST:
    case SHELF_BACKGROUND_OVERVIEW:
       shelf_target_color = darken_wallpaper(kShelfTranslucentColorDarkenAlpha);
       break;
     case SHELF_BACKGROUND_MAXIMIZED:
       shelf_target_color = darken_wallpaper(kShelfOpaqueColorDarkenAlpha);
       break;
     case SHELF_BACKGROUND_OOBE:
      shelf_target_color = SK_ColorTRANSPARENT;
      break;
    case SHELF_BACKGROUND_LOGIN:
      shelf_target_color = SK_ColorTRANSPARENT;
      break;
    case SHELF_BACKGROUND_LOGIN_NONBLURRED_WALLPAPER:
      shelf_target_color = login_constants::kDefaultBaseColor;
      break;
  }

  shelf_background_values->SetTargetValues(SkColorSetA(
      shelf_target_color, GetBackgroundAlphaValue(background_type)));
}
