 void AppResult::Open(int event_flags) {
   RecordHistogram(APP_SEARCH_RESULT);
   const extensions::Extension* extension =
      extensions::ExtensionRegistry::Get(profile_)->GetInstalledExtension(
          app_id_);
   if (!extension)
     return;
 
  if (!extensions::util::IsAppLaunchable(app_id_, profile_))
    return;

  if (RunExtensionEnableFlow())
    return;

  if (display_type() != DISPLAY_RECOMMENDATION) {
    extensions::RecordAppListSearchLaunch(extension);
    content::RecordAction(
        base::UserMetricsAction("AppList_ClickOnAppFromSearch"));
  }

  controller_->ActivateApp(
      profile_,
      extension,
      AppListControllerDelegate::LAUNCH_FROM_APP_LIST_SEARCH,
      event_flags);
}
