void LoginHtmlDialog::GetDialogSize(gfx::Size* size) const {
bool LoginHtmlDialog::ShouldShowDialogTitle() const {
  return true;
}

void LoginHtmlDialog::Observe(NotificationType type,
                              const NotificationSource& source,
                              const NotificationDetails& details) {
  DCHECK(type.value == NotificationType::LOAD_COMPLETED_MAIN_FRAME);
  if (bubble_frame_view_)
    bubble_frame_view_->StopThrobber();
 }
