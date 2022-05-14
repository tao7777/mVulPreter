 gfx::Size LauncherView::GetPreferredSize() {
   IdealBounds ideal_bounds;
   CalculateIdealBounds(&ideal_bounds);
   if (is_horizontal_alignment()) {
    if (view_model_->view_size() >= 2) {
      return gfx::Size(view_model_->ideal_bounds(1).right() + kLeadingInset,
                       kLauncherPreferredSize);
    }
    return gfx::Size(kLauncherPreferredSize * 2 + kLeadingInset * 2,
                      kLauncherPreferredSize);
   }
  if (view_model_->view_size() >= 2) {
    return gfx::Size(kLauncherPreferredSize,
                     view_model_->ideal_bounds(1).bottom() + kLeadingInset);
  }
   return gfx::Size(kLauncherPreferredSize,
                   kLauncherPreferredSize * 2 + kLeadingInset * 2);
 }
