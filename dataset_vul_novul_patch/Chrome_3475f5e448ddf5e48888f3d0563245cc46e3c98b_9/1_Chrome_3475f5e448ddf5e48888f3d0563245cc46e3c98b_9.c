 void LauncherView::UpdateFirstButtonPadding() {
   if (view_model_->view_size() > 0) {
     view_model_->view_at(0)->set_border(views::Border::CreateEmptyBorder(
        primary_axis_coordinate(0, kLeadingInset),
        primary_axis_coordinate(kLeadingInset, 0),
         0,
         0));
   }
}
