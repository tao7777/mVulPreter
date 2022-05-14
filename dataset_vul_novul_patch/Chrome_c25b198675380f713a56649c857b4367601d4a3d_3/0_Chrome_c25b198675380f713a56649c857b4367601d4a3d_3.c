 void LockScreenMediaControlsView::OnMouseExited(const ui::MouseEvent& event) {
   if (is_in_drag_ || contents_view_->layer()->GetAnimator()->is_animating())
     return;
 
  header_row_->SetCloseButtonVisibility(false);
 }
