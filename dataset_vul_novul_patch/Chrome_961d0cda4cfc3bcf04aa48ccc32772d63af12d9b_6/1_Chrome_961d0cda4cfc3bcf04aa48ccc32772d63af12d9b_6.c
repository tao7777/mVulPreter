void PasswordAccessoryControllerImpl::GeneratedPasswordAccepted(
    const base::string16& password) {
  if (!target_frame_driver_)
    return;
  RecordGenerationDialogDismissal(true);
  target_frame_driver_->GeneratedPasswordAccepted(password);
  dialog_view_.reset();
}
