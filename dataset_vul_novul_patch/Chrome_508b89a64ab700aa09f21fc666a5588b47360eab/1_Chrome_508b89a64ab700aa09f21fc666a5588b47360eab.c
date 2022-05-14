void AppListController::EnableAppList() {
   PrefService* local_state = g_browser_process->local_state();
  bool has_been_enabled = local_state->GetBoolean(
      apps::prefs::kAppLauncherHasBeenEnabled);
  if (!has_been_enabled) {
    local_state->SetBoolean(apps::prefs::kAppLauncherHasBeenEnabled, true);
    ShellIntegration::ShortcutLocations shortcut_locations;
    shortcut_locations.on_desktop = true;
    shortcut_locations.in_quick_launch_bar = true;
    shortcut_locations.in_applications_menu = true;
    BrowserDistribution* dist = BrowserDistribution::GetDistribution();
    shortcut_locations.applications_menu_subdir = dist->GetAppShortCutName();
    base::FilePath user_data_dir(
        g_browser_process->profile_manager()->user_data_dir());
    content::BrowserThread::PostTask(
        content::BrowserThread::FILE,
        FROM_HERE,
        base::Bind(&CreateAppListShortcuts,
                   user_data_dir, GetAppModelId(), shortcut_locations));
  }
 }
