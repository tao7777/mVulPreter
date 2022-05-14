void TranslateInfoBarBase::AnimationProgressed(const ui::Animation* animation) {
   if (background_color_animation_.get() == animation)
     SchedulePaint();  // That'll trigger a PaintBackgroud.
   else
    InfoBarView::AnimationProgressed(animation);
 }
