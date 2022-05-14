void TabSpecificContentSettings::OnContentBlocked(
    ContentSettingsType type,
     const std::string& resource_identifier) {
   DCHECK(type != CONTENT_SETTINGS_TYPE_GEOLOCATION)
       << "Geolocation settings handled by OnGeolocationPermissionSet";
  if (type < 0 || type >= CONTENT_SETTINGS_NUM_TYPES)
    return;
   content_accessed_[type] = true;
  std::string identifier;
  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableResourceContentSettings)) {
    identifier = resource_identifier;
  }
  if (!identifier.empty())
    AddBlockedResource(type, identifier);

#if defined (OS_ANDROID)
  if (type == CONTENT_SETTINGS_TYPE_POPUPS) {
    content_blocked_[type] = false;
    content_blockage_indicated_to_user_[type] = false;
  }
#endif

  if (!content_blocked_[type]) {
    content_blocked_[type] = true;
    content::NotificationService::current()->Notify(
        chrome::NOTIFICATION_WEB_CONTENT_SETTINGS_CHANGED,
        content::Source<WebContents>(web_contents()),
        content::NotificationService::NoDetails());
  }
}
