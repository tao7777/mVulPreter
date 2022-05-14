 void LauncherView::OnBoundsChanged(const gfx::Rect& previous_bounds) {
   LayoutToIdealBounds();
   FOR_EACH_OBSERVER(LauncherIconObserver, observers_,
                     OnLauncherIconPositionsChanged());
 }
