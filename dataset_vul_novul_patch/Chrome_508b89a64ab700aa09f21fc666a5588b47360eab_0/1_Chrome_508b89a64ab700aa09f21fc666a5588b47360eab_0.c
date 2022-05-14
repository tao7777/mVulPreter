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
 
   AppListController::GetInstance();
 
  ScheduleWarmup();

  MigrateAppLauncherEnabledPref();

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableAppList))
    EnableAppList();

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDisableAppList))
    DisableAppList();
}
