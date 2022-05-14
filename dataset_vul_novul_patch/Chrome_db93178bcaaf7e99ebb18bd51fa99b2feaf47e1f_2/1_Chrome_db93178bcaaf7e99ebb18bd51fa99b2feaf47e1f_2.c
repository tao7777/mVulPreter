 void UninstallExtension(ExtensionService* service, const std::string& id) {
  if (service && service->GetInstalledExtension(id)) {
    service->UninstallExtension(id,
                                extensions::UNINSTALL_REASON_SYNC,
                                base::Bind(&base::DoNothing),
                                NULL);
   }
 }
