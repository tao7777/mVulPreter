 const Extension* ExtensionAppItem::GetExtension() const {
  const ExtensionService* service =
      extensions::ExtensionSystem::Get(profile_)->extension_service();
  const Extension* extension = service->GetInstalledExtension(extension_id_);
   return extension;
 }
