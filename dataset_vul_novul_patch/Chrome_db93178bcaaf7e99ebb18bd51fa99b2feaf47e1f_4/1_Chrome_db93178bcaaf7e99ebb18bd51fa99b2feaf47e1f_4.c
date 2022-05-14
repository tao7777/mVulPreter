 void ExtensionUninstaller::Run() {
   const extensions::Extension* extension =
      extensions::ExtensionSystem::Get(profile_)->extension_service()->
          GetInstalledExtension(app_id_);
   if (!extension) {
     CleanUp();
     return;
  }
  controller_->OnShowChildDialog();
  dialog_.reset(extensions::ExtensionUninstallDialog::Create(
      profile_, controller_->GetAppListWindow(), this));
  dialog_->ConfirmUninstall(extension,
                            extensions::UNINSTALL_REASON_USER_INITIATED);
}
