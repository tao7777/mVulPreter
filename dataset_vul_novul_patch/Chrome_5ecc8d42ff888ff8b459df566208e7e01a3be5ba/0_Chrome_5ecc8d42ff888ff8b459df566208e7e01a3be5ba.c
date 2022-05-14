 void ColorChooserDialog::DidCloseDialog(bool chose_color,
                                         SkColor color,
                                         RunState run_state) {
   EndRun(run_state);
   CopyCustomColors(custom_colors_, g_custom_colors);
  if (listener_) {
    if (chose_color)
      listener_->OnColorChosen(color);
    listener_->OnColorChooserDialogClosed();
  }
 }
