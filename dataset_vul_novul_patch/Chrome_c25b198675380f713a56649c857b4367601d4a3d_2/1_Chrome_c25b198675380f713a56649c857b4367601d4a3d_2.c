void LockScreenMediaControlsView::OnMouseEntered(const ui::MouseEvent& event) {
   if (is_in_drag_ || contents_view_->layer()->GetAnimator()->is_animating())
     return;
 
  close_button_->SetVisible(true);
 }
