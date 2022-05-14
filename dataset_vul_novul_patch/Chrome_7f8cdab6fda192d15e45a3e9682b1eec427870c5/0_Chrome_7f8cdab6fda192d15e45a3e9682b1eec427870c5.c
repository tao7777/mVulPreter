 void ShellWindowFrameView::ButtonPressed(views::Button* sender,
                                          const views::Event& event) {
  DCHECK(!is_frameless_);
   if (sender == close_button_)
     frame_->Close();
 }
