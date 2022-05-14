void ProfileSyncComponentsFactoryImpl::RegisterDataTypes(
    ProfileSyncService* pss) {
  if (!command_line_->HasSwitch(switches::kDisableSyncApps)) {
    pss->RegisterDataTypeController(
        new ExtensionDataTypeController(syncable::APPS, this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncAutofill)) {
    pss->RegisterDataTypeController(
        new AutofillDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncBookmarks)) {
    pss->RegisterDataTypeController(
        new BookmarkDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncExtensions)) {
    pss->RegisterDataTypeController(
        new ExtensionDataTypeController(syncable::EXTENSIONS,
                                        this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncPasswords)) {
    pss->RegisterDataTypeController(
        new PasswordDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncPreferences)) {
    pss->RegisterDataTypeController(
        new UIDataTypeController(syncable::PREFERENCES, this, profile_, pss));
  }

#if defined(ENABLE_THEMES)
  if (!command_line_->HasSwitch(switches::kDisableSyncThemes)) {
    pss->RegisterDataTypeController(
        new ThemeDataTypeController(this, profile_, pss));
  }
#endif

  if (!profile_->GetPrefs()->GetBoolean(prefs::kSavingBrowserHistoryDisabled) &&
      !command_line_->HasSwitch(switches::kDisableSyncTypedUrls)) {
    pss->RegisterDataTypeController(
        new TypedUrlDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncSearchEngines)) {
    pss->RegisterDataTypeController(
        new SearchEngineDataTypeController(this, profile_, pss));
   }
 
     pss->RegisterDataTypeController(
         new SessionDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncExtensionSettings)) {
    pss->RegisterDataTypeController(
        new ExtensionSettingDataTypeController(
            syncable::EXTENSION_SETTINGS, this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncAppSettings)) {
    pss->RegisterDataTypeController(
        new ExtensionSettingDataTypeController(
            syncable::APP_SETTINGS, this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncAutofillProfile)) {
    pss->RegisterDataTypeController(
        new AutofillProfileDataTypeController(this, profile_, pss));
  }

  if (!command_line_->HasSwitch(switches::kDisableSyncAppNotifications)) {
    pss->RegisterDataTypeController(
        new AppNotificationDataTypeController(this, profile_, pss));
  }
}
