void WebstoreStandaloneInstaller::ShowInstallUI() {
   scoped_refptr<const Extension> localized_extension =
       GetLocalizedExtensionForDisplay();
   if (!localized_extension.get()) {
    CompleteInstall(webstore_install::INVALID_MANIFEST, kInvalidManifestError);
     return;
   }
 
  install_ui_ = CreateInstallUI();
  install_ui_->ShowDialog(
      base::Bind(&WebstoreStandaloneInstaller::OnInstallPromptDone, this),
      localized_extension.get(), &icon_, std::move(install_prompt_),
      ExtensionInstallPrompt::GetDefaultShowDialogCallback());
}
