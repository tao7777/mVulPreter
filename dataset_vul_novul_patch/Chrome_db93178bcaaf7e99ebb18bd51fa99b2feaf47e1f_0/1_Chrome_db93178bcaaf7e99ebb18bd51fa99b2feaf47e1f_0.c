 const extensions::Extension* GetExtension(Profile* profile,
                                           const std::string& extension_id) {
  const ExtensionService* service =
      extensions::ExtensionSystem::Get(profile)->extension_service();
   const extensions::Extension* extension =
      service->GetInstalledExtension(extension_id);
   return extension;
 }
