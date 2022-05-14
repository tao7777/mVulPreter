void ProfileSyncComponentsFactoryImpl::RegisterDesktopDataTypes(
    ProfileSyncService* pss) {
  if (!command_line_->HasSwitch(switches::kDisableSyncApps)) {
    pss->RegisterDataTypeController(
        new ExtensionDataTypeController(syncer::APPS, this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncExtensions)) {
    pss->RegisterDataTypeController(
        new ExtensionDataTypeController(syncer::EXTENSIONS,
                                        this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncPreferences)) {
    pss->RegisterDataTypeController(
        new UIDataTypeController(syncer::PREFERENCES, this, profile_, pss));
  }

#if defined(ENABLE_THEMES)
  if (!command_line_->HasSwitch(switches::kDisableSyncThemes)) {
    pss->RegisterDataTypeController(
        new ThemeDataTypeController(this, profile_, pss));
  }
#endif

  if (!command_line_->HasSwitch(switches::kDisableSyncSearchEngines)) {
    pss->RegisterDataTypeController(
        new SearchEngineDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncExtensionSettings)) {
    pss->RegisterDataTypeController(
        new ExtensionSettingDataTypeController(
            syncer::EXTENSION_SETTINGS, this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncAppSettings)) {
    pss->RegisterDataTypeController(
        new ExtensionSettingDataTypeController(
            syncer::APP_SETTINGS, this, profile_, pss));
  }
 
  if (command_line_->HasSwitch(switches::kEnableSyncSyncedNotifications)) {
#if !defined(OS_ANDROID)
    pss->RegisterDataTypeController(
        new UIDataTypeController(
            syncer::SYNCED_NOTIFICATIONS, this, profile_, pss));
#endif
   }
 
 #if defined(OS_LINUX) || defined(OS_WIN) || defined(OS_CHROMEOS)
  if (!command_line_->HasSwitch(switches::kDisableSyncDictionary)) {
    pss->RegisterDataTypeController(
        new UIDataTypeController(syncer::DICTIONARY, this, profile_, pss));
  }
#endif

}
