 void SaveCCInfoConfirmInfoBar::ButtonPressed(views::Button* sender,
                                              const views::Event& event) {
  InfoBarView::ButtonPressed(sender, event);
   if (sender == save_button_) {
     if (GetDelegate()->Accept())
       RemoveInfoBar();
  } else if (sender == dont_save_button_) {
    if (GetDelegate()->Cancel())
      RemoveInfoBar();
  }
}
