SyncType GetSyncType(const Extension* extension) {
  if (!IsSyncable(extension)) {
    return SYNC_TYPE_NONE;
  }

  if (!ManifestURL::GetUpdateURL(extension).is_empty() &&
      !ManifestURL::UpdatesFromGallery(extension)) {
    return SYNC_TYPE_NONE;
  }

  if (PluginInfo::HasPlugins(extension) ||
      extension->HasAPIPermission(APIPermission::kPlugin)) {
     return SYNC_TYPE_NONE;
  }
 
   switch (extension->GetType()) {
     case Manifest::TYPE_EXTENSION:
      return SYNC_TYPE_EXTENSION;

    case Manifest::TYPE_USER_SCRIPT:
      if (ManifestURL::UpdatesFromGallery(extension))
        return SYNC_TYPE_EXTENSION;
      return SYNC_TYPE_NONE;

    case Manifest::TYPE_HOSTED_APP:
    case Manifest::TYPE_LEGACY_PACKAGED_APP:
    case Manifest::TYPE_PLATFORM_APP:
      return SYNC_TYPE_APP;

    case Manifest::TYPE_UNKNOWN:
    case Manifest::TYPE_THEME:
    case Manifest::TYPE_SHARED_MODULE:
      return SYNC_TYPE_NONE;
  }
  NOTREACHED();
  return SYNC_TYPE_NONE;
}
