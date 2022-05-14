 void UninstallExtension(ExtensionService* service, const std::string& id) {
  if (service) {
    ExtensionService::UninstallExtensionHelper(
        service, id, extensions::UNINSTALL_REASON_SYNC);
   }
 }
