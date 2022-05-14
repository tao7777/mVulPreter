void ShellWindow::Observe(int type,
                           const content::NotificationSource& source,
                           const content::NotificationDetails& details) {
   switch (type) {
    case content::NOTIFICATION_RENDER_VIEW_HOST_CHANGED: {
      // TODO(jeremya): once http://crbug.com/123007 is fixed, we'll no longer
      // need to suspend resource requests here (the call in the constructor
      // should be enough).
      content::Details<std::pair<RenderViewHost*, RenderViewHost*> >
          host_details(details);
      if (host_details->first)
        SuspendRenderViewHost(host_details->second);
      break;
    }
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
