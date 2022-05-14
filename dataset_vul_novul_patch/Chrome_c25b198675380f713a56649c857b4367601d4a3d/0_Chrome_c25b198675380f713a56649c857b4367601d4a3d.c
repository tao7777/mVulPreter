 void LockScreenMediaControlsView::ButtonPressed(views::Button* sender,
                                                 const ui::Event& event) {
   if (!base::Contains(enabled_actions_,
                       media_message_center::GetActionFromButtonTag(*sender)) ||
       !media_session_id_.has_value()) {
    return;
  }

  media_session::PerformMediaSessionAction(
      media_message_center::GetActionFromButtonTag(*sender),
      media_controller_remote_);
}
