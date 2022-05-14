void ContentSettingsStore::ClearContentSettingsForExtension(
    const std::string& ext_id,
    ExtensionPrefsScope scope) {
  bool notify = false;
   {
     base::AutoLock lock(lock_);
     OriginIdentifierValueMap* map = GetValueMap(ext_id, scope);
      // Fail gracefully in Release builds.
      NOTREACHED();
      return;
     }
     notify = !map->empty();
     map->clear();
  }
  if (notify) {
    NotifyOfContentSettingChanged(ext_id, scope != kExtensionPrefsScopeRegular);
  }
}
