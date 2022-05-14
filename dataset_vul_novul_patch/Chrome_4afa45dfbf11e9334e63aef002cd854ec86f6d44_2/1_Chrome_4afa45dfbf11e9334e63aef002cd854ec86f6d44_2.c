 void BrowserActionsContainer::Observe(NotificationType type,
                                       const NotificationSource& source,
                                       const NotificationDetails& details) {
   switch (type.value) {
     case NotificationType::EXTENSION_HOST_VIEW_SHOULD_CLOSE:
       if (!popup_ || Details<ExtensionHost>(popup_->host()) != details)
        return;

      HidePopup();
      break;

    default:
      NOTREACHED() << "Unexpected notification";
  }
}
