 void LauncherView::OnBoundsAnimatorProgressed(views::BoundsAnimator* animator) {
   FOR_EACH_OBSERVER(LauncherIconObserver, observers_,
                     OnLauncherIconPositionsChanged());
 }
