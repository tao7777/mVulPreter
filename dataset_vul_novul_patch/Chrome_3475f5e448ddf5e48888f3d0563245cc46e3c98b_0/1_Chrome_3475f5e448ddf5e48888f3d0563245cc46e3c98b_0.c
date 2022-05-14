void LauncherView::CalculateIdealBounds(IdealBounds* bounds) {
  int available_size = primary_axis_coordinate(width(), height());
   if (!available_size)
     return;
 
  int x = primary_axis_coordinate(kLeadingInset, 0);
  int y = primary_axis_coordinate(0, kLeadingInset);
   for (int i = 0; i < view_model_->view_size(); ++i) {
     view_model_->set_ideal_bounds(i, gfx::Rect(
         x, y, kLauncherPreferredSize, kLauncherPreferredSize));
     x = primary_axis_coordinate(x + kLauncherPreferredSize + kButtonSpacing, 0);
     y = primary_axis_coordinate(0, y + kLauncherPreferredSize + kButtonSpacing);
   }
 
   if (view_model_->view_size() > 0) {
     view_model_->set_ideal_bounds(0, gfx::Rect(gfx::Size(
        primary_axis_coordinate(kLeadingInset + kLauncherPreferredSize,
                                 kLauncherPreferredSize),
         primary_axis_coordinate(kLauncherPreferredSize,
                                kLeadingInset + kLauncherPreferredSize))));
   }
 
   bounds->overflow_bounds.set_size(
       gfx::Size(kLauncherPreferredSize, kLauncherPreferredSize));
   last_visible_index_ = DetermineLastVisibleIndex(
      available_size - kLeadingInset - kLauncherPreferredSize -
       kButtonSpacing - kLauncherPreferredSize);
  int app_list_index = view_model_->view_size() - 1;
   bool show_overflow = (last_visible_index_ + 1 < app_list_index);
 
   for (int i = 0; i < view_model_->view_size(); ++i) {
    view_model_->view_at(i)->SetVisible(
        i == app_list_index || i <= last_visible_index_);
  }

  overflow_button_->SetVisible(show_overflow);
   if (show_overflow) {
     DCHECK_NE(0, view_model_->view_size());
     if (last_visible_index_ == -1) {
      x = primary_axis_coordinate(kLeadingInset, 0);
      y = primary_axis_coordinate(0, kLeadingInset);
     } else {
       x = primary_axis_coordinate(
           view_model_->ideal_bounds(last_visible_index_).right(), 0);
       y = primary_axis_coordinate(0,
           view_model_->ideal_bounds(last_visible_index_).bottom());
     }
     gfx::Rect app_list_bounds = view_model_->ideal_bounds(app_list_index);
     app_list_bounds.set_x(x);
     app_list_bounds.set_y(y);
     view_model_->set_ideal_bounds(app_list_index, app_list_bounds);
    x = primary_axis_coordinate(x + kLauncherPreferredSize + kButtonSpacing, 0);
    y = primary_axis_coordinate(0, y + kLauncherPreferredSize + kButtonSpacing);
    bounds->overflow_bounds.set_x(x);
    bounds->overflow_bounds.set_y(y);
   }
 }
