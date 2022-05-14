void ShellWindow::Observe(int type,
                           const content::NotificationSource& source,
                           const content::NotificationDetails& details) {
   switch (type) {
     case chrome::NOTIFICATION_EXTENSION_UNLOADED: {
       const extensions::Extension* unloaded_extension =
           content::Details<extensions::UnloadedExtensionInfo>(
              details)->extension;
      if (extension_ == unloaded_extension)
        Close();
      break;
    }
    case content::NOTIFICATION_APP_TERMINATING:
      Close();
      break;
    default:
      NOTREACHED() << "Received unexpected notification";
  }
}
