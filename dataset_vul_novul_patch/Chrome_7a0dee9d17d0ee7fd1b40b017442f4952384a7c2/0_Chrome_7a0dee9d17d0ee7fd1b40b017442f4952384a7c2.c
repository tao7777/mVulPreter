SessionStartupPref StartupBrowserCreator::GetSessionStartupPref(
    const base::CommandLine& command_line,
    Profile* profile) {
  DCHECK(profile);
  PrefService* prefs = profile->GetPrefs();
  SessionStartupPref pref = SessionStartupPref::GetStartupPref(prefs);

#if defined(OS_CHROMEOS)
  const bool is_first_run =
      user_manager::UserManager::Get()->IsCurrentUserNew();
  const bool did_restart = false;
  StartupBrowserCreator::WasRestarted();
#else
  const bool is_first_run = first_run::IsChromeFirstRun();
  const bool did_restart = StartupBrowserCreator::WasRestarted();
#endif

  if (is_first_run && SessionStartupPref::TypeIsDefault(prefs))
    pref.type = SessionStartupPref::DEFAULT;

  if ((command_line.HasSwitch(switches::kRestoreLastSession) || did_restart) &&
      !profile->IsNewProfile()) {
    pref.type = SessionStartupPref::LAST;
  }

  if (!profile->IsGuestSession()) {
    ProfileAttributesEntry* entry = nullptr;
    bool has_entry =
        g_browser_process->profile_manager()
            ->GetProfileAttributesStorage()
            .GetProfileAttributesWithPath(profile->GetPath(), &entry);

    if (has_entry && entry->IsSigninRequired())
       pref.type = SessionStartupPref::LAST;
   }
 
  if (pref.type == SessionStartupPref::LAST && profile->IsOffTheRecord()) {
    pref.type = SessionStartupPref::DEFAULT;
  }

  return pref;
}
