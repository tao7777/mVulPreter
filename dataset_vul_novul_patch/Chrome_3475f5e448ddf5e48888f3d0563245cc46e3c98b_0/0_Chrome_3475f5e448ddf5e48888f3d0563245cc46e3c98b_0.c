void LauncherView::CalculateIdealBounds(IdealBounds* bounds) {
  int available_size = primary_axis_coordinate(width(), height());
   if (!available_size)
     return;
 
  int x = primary_axis_coordinate(leading_inset(), 0);
  int y = primary_axis_coordinate(0, leading_inset());
   for (int i = 0; i < view_model_->view_size(); ++i) {
    if (i < first_visible_index_) {
      view_model_->set_ideal_bounds(i, gfx::Rect(x, y, 0, 0));
      continue;
    }

     view_model_->set_ideal_bounds(i, gfx::Rect(
         x, y, kLauncherPreferredSize, kLauncherPreferredSize));
     x = primary_axis_coordinate(x + kLauncherPreferredSize + kButtonSpacing, 0);
     y = primary_axis_coordinate(0, y + kLauncherPreferredSize + kButtonSpacing);
   }
 
  int app_list_index = view_model_->view_size() - 1;
  if (is_overflow_mode()) {
    last_visible_index_ = app_list_index - 1;
    for (int i = 0; i < view_model_->view_size(); ++i) {
      view_model_->view_at(i)->SetVisible(
          i >= first_visible_index_ && i <= last_visible_index_);
    }
    return;
  }

   if (view_model_->view_size() > 0) {
     view_model_->set_ideal_bounds(0, gfx::Rect(gfx::Size(
        primary_axis_coordinate(leading_inset() + kLauncherPreferredSize,
                                 kLauncherPreferredSize),
         primary_axis_coordinate(kLauncherPreferredSize,
                                leading_inset() + kLauncherPreferredSize))));
   }
 
   bounds->overflow_bounds.set_size(
       gfx::Size(kLauncherPreferredSize, kLauncherPreferredSize));
   last_visible_index_ = DetermineLastVisibleIndex(
      available_size - leading_inset() - kLauncherPreferredSize -
       kButtonSpacing - kLauncherPreferredSize);
   bool show_overflow = (last_visible_index_ + 1 < app_list_index);
 
   for (int i = 0; i < view_model_->view_size(); ++i) {
    view_model_->view_at(i)->SetVisible(
        i == app_list_index || i <= last_visible_index_);
  }

  overflow_button_->SetVisible(show_overflow);
   if (show_overflow) {
     DCHECK_NE(0, view_model_->view_size());
     if (last_visible_index_ == -1) {
      x = primary_axis_coordinate(leading_inset(), 0);
      y = primary_axis_coordinate(0, leading_inset());
     } else {
       x = primary_axis_coordinate(
           view_model_->ideal_bounds(last_visible_index_).right(), 0);
       y = primary_axis_coordinate(0,
           view_model_->ideal_bounds(last_visible_index_).bottom());
     }
     gfx::Rect app_list_bounds = view_model_->ideal_bounds(app_list_index);
    bounds->overflow_bounds.set_x(x);
    bounds->overflow_bounds.set_y(y);
    x = primary_axis_coordinate(x + kLauncherPreferredSize + kButtonSpacing, 0);
    y = primary_axis_coordinate(0, y + kLauncherPreferredSize + kButtonSpacing);
     app_list_bounds.set_x(x);
     app_list_bounds.set_y(y);
     view_model_->set_ideal_bounds(app_list_index, app_list_bounds);
  } else {
    if (overflow_bubble_.get())
      overflow_bubble_->Hide();
   }
 }
