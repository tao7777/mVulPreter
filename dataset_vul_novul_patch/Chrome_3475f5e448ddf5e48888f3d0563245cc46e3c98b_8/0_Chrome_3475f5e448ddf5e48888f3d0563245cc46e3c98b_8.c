void LauncherView::ShowOverflowMenu() {
void LauncherView::ShowOverflowBubble() {
  int first_overflow_index = last_visible_index_ + 1;
  DCHECK_LT(first_overflow_index, view_model_->view_size() - 1);
 
  if (!overflow_bubble_.get())
    overflow_bubble_.reset(new OverflowBubble());
 
  overflow_bubble_->Show(delegate_,
                         model_,
                         overflow_button_,
                         alignment_,
                         first_overflow_index);
 
   Shell::GetInstance()->UpdateShelfVisibility();
 }
