 void AppShortcutManager::OnceOffCreateShortcuts() {
  if (prefs_->GetBoolean(prefs::kAppShortcutsHaveBeenCreated))
     return;
 
  prefs_->SetBoolean(prefs::kAppShortcutsHaveBeenCreated, true);

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
