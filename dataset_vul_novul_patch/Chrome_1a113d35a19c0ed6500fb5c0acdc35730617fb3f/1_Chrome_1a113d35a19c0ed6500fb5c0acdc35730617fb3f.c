void ContentSettingsStore::ClearContentSettingsForExtension(
    const std::string& ext_id,
    ExtensionPrefsScope scope) {
  bool notify = false;
   {
     base::AutoLock lock(lock_);
     OriginIdentifierValueMap* map = GetValueMap(ext_id, scope);
      char ext_id_buffer[33];
      base::strlcpy(ext_id_buffer, ext_id.c_str(), sizeof(ext_id_buffer));
      base::debug::Alias(ext_id_buffer);
      CHECK(false);
     }
     notify = !map->empty();
     map->clear();
  }
  if (notify) {
    NotifyOfContentSettingChanged(ext_id, scope != kExtensionPrefsScopeRegular);
  }
}
