 gfx::Size LauncherView::GetPreferredSize() {
   IdealBounds ideal_bounds;
   CalculateIdealBounds(&ideal_bounds);

  const int app_list_index = view_model_->view_size() - 1;
  const int last_button_index = is_overflow_mode() ?
      last_visible_index_ : app_list_index;
  const gfx::Rect last_button_bounds =
      last_button_index  >= first_visible_index_ ?
          view_model_->view_at(last_button_index)->bounds() :
          gfx::Rect(gfx::Size(kLauncherPreferredSize,
                              kLauncherPreferredSize));

   if (is_horizontal_alignment()) {
    return gfx::Size(last_button_bounds.right() + leading_inset(),
                      kLauncherPreferredSize);
   }

   return gfx::Size(kLauncherPreferredSize,
                   last_button_bounds.bottom() + leading_inset());
 }
