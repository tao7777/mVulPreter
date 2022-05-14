 TranslateInfoBarBase::TranslateInfoBarBase(
     TranslateInfoBarDelegate* delegate)
    : InfoBar(delegate),
       normal_background_(InfoBarDelegate::PAGE_ACTION_TYPE),
       error_background_(InfoBarDelegate::WARNING_TYPE) {
   icon_ = new views::ImageView;
  SkBitmap* image = static_cast<InfoBarDelegate*>(delegate)->GetIcon();
  if (image)
    icon_->SetImage(image);
  AddChildView(icon_);

  TranslateInfoBarDelegate::BackgroundAnimationType animation =
      delegate->background_animation_type();
  if (animation != TranslateInfoBarDelegate::NONE) {
    background_color_animation_.reset(new ui::SlideAnimation(this));
    background_color_animation_->SetTweenType(ui::Tween::LINEAR);
    background_color_animation_->SetSlideDuration(500);
    if (animation == TranslateInfoBarDelegate::NORMAL_TO_ERROR) {
      background_color_animation_->Show();
    } else {
      DCHECK_EQ(TranslateInfoBarDelegate::ERROR_TO_NORMAL, animation);
      background_color_animation_->Reset(1.0);
      background_color_animation_->Hide();
    }
  }
}
