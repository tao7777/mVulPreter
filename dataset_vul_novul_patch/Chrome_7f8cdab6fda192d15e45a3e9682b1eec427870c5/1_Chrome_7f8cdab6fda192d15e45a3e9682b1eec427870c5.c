 void ShellWindowFrameView::ButtonPressed(views::Button* sender,
                                          const views::Event& event) {
   if (sender == close_button_)
     frame_->Close();
 }
