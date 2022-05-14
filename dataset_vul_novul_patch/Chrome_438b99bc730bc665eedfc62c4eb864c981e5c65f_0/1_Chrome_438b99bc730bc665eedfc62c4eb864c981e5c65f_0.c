 void AppShortcutManager::OnceOffCreateShortcuts() {
  bool was_enabled = prefs_->GetBoolean(prefs::kAppShortcutsHaveBeenCreated);
#if defined(OS_MACOSX)
  bool is_now_enabled = apps::IsAppShimsEnabled();
#else
  bool is_now_enabled = true;
#endif  // defined(OS_MACOSX)
  if (was_enabled != is_now_enabled)
    prefs_->SetBoolean(prefs::kAppShortcutsHaveBeenCreated, is_now_enabled);
  if (was_enabled || !is_now_enabled)
     return;
 
   extensions::ExtensionSystem* extension_system;
  ExtensionServiceInterface* extension_service;
  if (!(extension_system = extensions::ExtensionSystem::Get(profile_)) ||
      !(extension_service = extension_system->extension_service()))
    return;

  const extensions::ExtensionSet* apps = extension_service->extensions();
  for (extensions::ExtensionSet::const_iterator it = apps->begin();
       it != apps->end(); ++it) {
    if (ShouldCreateShortcutFor(profile_, it->get()))
      CreateShortcutsInApplicationsMenu(profile_, it->get());
  }
}
