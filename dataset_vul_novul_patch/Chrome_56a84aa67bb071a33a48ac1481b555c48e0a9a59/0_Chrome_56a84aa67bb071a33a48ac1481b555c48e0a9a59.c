void NavigationObserver::PromptToEnableExtensionIfNecessary(
    NavigationController* nav_controller) {
  if (!in_progress_prompt_extension_id_.empty())
    return;

  NavigationEntry* nav_entry = nav_controller->GetVisibleEntry();
   if (!nav_entry)
     return;
 
  const GURL& url = nav_entry->GetURL();
 
  if (!url.SchemeIs(kExtensionScheme))
    return;

  const Extension* extension = ExtensionRegistry::Get(profile_)
                                   ->disabled_extensions()
                                   .GetExtensionOrAppByURL(url);
  if (!extension)
    return;

  if (!prompted_extensions_.insert(extension->id()).second &&
      !g_repeat_prompting) {
    return;
  }

  ExtensionPrefs* extension_prefs = ExtensionPrefs::Get(profile_);
  if (extension_prefs->DidExtensionEscalatePermissions(extension->id())) {
    in_progress_prompt_extension_id_ = extension->id();
    in_progress_prompt_navigation_controller_ = nav_controller;

    extension_install_prompt_.reset(
        new ExtensionInstallPrompt(nav_controller->GetWebContents()));
    ExtensionInstallPrompt::PromptType type =
        ExtensionInstallPrompt::GetReEnablePromptTypeForExtension(profile_,
                                                                  extension);
    extension_install_prompt_->ShowDialog(
        base::Bind(&NavigationObserver::OnInstallPromptDone,
                   weak_factory_.GetWeakPtr()),
        extension, nullptr,
        base::MakeUnique<ExtensionInstallPrompt::Prompt>(type),
        ExtensionInstallPrompt::GetDefaultShowDialogCallback());
  }
}
