 const Extension* ExtensionAppItem::GetExtension() const {
  const extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(profile_);
  const Extension* extension = registry->GetInstalledExtension(
      extension_id_);
   return extension;
 }
