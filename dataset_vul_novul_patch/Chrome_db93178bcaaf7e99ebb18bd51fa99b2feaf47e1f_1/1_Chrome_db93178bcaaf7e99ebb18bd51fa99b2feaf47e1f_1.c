void AppListControllerDelegateImpl::DoCreateShortcutsFlow(
     Profile* profile,
     const std::string& extension_id) {
   DCHECK(CanDoCreateShortcutsFlow());
  ExtensionService* service =
      extensions::ExtensionSystem::Get(profile)->extension_service();
  DCHECK(service);
  const extensions::Extension* extension = service->GetInstalledExtension(
       extension_id);
   DCHECK(extension);
 
  gfx::NativeWindow parent_window = GetAppListWindow();
  if (!parent_window)
    return;
  OnShowChildDialog();
  chrome::ShowCreateChromeAppShortcutsDialog(
      parent_window, profile, extension,
      base::Bind(&AppListControllerDelegateImpl::OnCloseCreateShortcutsPrompt,
                 base::Unretained(this)));
}
