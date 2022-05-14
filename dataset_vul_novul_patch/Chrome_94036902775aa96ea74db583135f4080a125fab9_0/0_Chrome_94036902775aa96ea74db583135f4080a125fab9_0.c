 void AppModalDialog::CompleteDialog() {
  if (!completed_) {
    completed_ = true;
    AppModalDialogQueue::GetInstance()->ShowNextDialog();
  }
 }
