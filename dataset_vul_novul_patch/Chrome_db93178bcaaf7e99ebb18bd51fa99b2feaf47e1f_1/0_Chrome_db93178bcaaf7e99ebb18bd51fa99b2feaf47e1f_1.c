void AppListControllerDelegateImpl::DoCreateShortcutsFlow(
     Profile* profile,
     const std::string& extension_id) {
   DCHECK(CanDoCreateShortcutsFlow());
  extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(profile);
  const extensions::Extension* extension = registry->GetInstalledExtension(
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
