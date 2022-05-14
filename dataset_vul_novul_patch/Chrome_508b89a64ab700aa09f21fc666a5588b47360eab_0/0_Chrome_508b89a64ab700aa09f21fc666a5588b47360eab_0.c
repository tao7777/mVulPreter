void AppListController::Init(Profile* initial_profile) {
  if (win8::IsSingleWindowMetroMode())
    return;

  PrefService* prefs = g_browser_process->local_state();
  if (prefs->HasPrefPath(prefs::kRestartWithAppList) &&
      prefs->GetBoolean(prefs::kRestartWithAppList)) {
    prefs->SetBoolean(prefs::kRestartWithAppList, false);
    AppListController::GetInstance()->
         ShowAppListDuringModeSwitch(initial_profile);
   }
 
  // Migrate from legacy app launcher if we are on a non-canary and non-chromium
  // build.
#if defined(GOOGLE_CHROME_BUILD)
  if (!InstallUtil::IsChromeSxSProcess() &&
      !chrome_launcher_support::GetAnyAppHostPath().empty()) {
    chrome_launcher_support::InstallationState state =
        chrome_launcher_support::GetAppLauncherInstallationState();
    if (state == chrome_launcher_support::NOT_INSTALLED) {
      // If app_host.exe is found but can't be located in the registry,
      // skip the migration as this is likely a developer build.
      return;
    } else if (state == chrome_launcher_support::INSTALLED_AT_SYSTEM_LEVEL) {
      chrome_launcher_support::UninstallLegacyAppLauncher(
          chrome_launcher_support::SYSTEM_LEVEL_INSTALLATION);
    } else if (state == chrome_launcher_support::INSTALLED_AT_USER_LEVEL) {
      chrome_launcher_support::UninstallLegacyAppLauncher(
          chrome_launcher_support::USER_LEVEL_INSTALLATION);
    }
    EnableAppList();
  }
#endif

   AppListController::GetInstance();
 
  ScheduleWarmup();

  MigrateAppLauncherEnabledPref();

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableAppList))
    EnableAppList();

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDisableAppList))
    DisableAppList();
}
