void ShelfBackgroundAnimator::CreateAnimator(
    ShelfBackgroundType background_type) {
  int duration_ms = 0;

  switch (background_type) {
    case SHELF_BACKGROUND_DEFAULT:
    case SHELF_BACKGROUND_APP_LIST:
       duration_ms = 500;
       break;
     case SHELF_BACKGROUND_MAXIMIZED:
     case SHELF_BACKGROUND_OOBE:
     case SHELF_BACKGROUND_LOGIN:
     case SHELF_BACKGROUND_LOGIN_NONBLURRED_WALLPAPER:
    case SHELF_BACKGROUND_OVERVIEW:
      duration_ms = 250;
      break;
  }

  animator_ = std::make_unique<gfx::SlideAnimation>(this);
  animator_->SetSlideDuration(duration_ms);
}
