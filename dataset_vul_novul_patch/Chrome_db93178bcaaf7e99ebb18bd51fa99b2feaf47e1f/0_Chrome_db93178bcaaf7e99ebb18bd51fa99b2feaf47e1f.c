void AppListControllerDelegate::DoShowAppInfoFlow(
     Profile* profile,
     const std::string& extension_id) {
   DCHECK(CanDoShowAppInfoFlow());
  const extensions::Extension* extension = GetExtension(profile, extension_id);
   DCHECK(extension);
 
   OnShowChildDialog();

  UMA_HISTOGRAM_ENUMERATION("Apps.AppInfoDialog.Launches",
                            AppInfoLaunchSource::FROM_APP_LIST,
                            AppInfoLaunchSource::NUM_LAUNCH_SOURCES);

  ShowAppInfoInAppList(
      GetAppListWindow(),
      GetAppListBounds(),
      profile,
      extension,
      base::Bind(&AppListControllerDelegate::OnCloseChildDialog,
                 base::Unretained(this)));
}
