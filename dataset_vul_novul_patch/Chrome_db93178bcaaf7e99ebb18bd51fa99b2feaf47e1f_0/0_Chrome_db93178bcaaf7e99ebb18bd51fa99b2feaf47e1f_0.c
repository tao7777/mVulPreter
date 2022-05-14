 const extensions::Extension* GetExtension(Profile* profile,
                                           const std::string& extension_id) {
  const ExtensionRegistry* registry = ExtensionRegistry::Get(profile);
   const extensions::Extension* extension =
      registry->GetInstalledExtension(extension_id);
   return extension;
 }
