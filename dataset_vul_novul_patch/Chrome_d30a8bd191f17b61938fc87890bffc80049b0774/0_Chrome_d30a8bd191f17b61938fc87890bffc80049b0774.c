void EnsureBrowserContextKeyedServiceFactoriesBuilt() {
  extensions::ActivityLog::GetFactoryInstance();
  extensions::ActivityLogAPI::GetFactoryInstance();
  extensions::AutofillPrivateEventRouterFactory::GetInstance();
  extensions::BluetoothLowEnergyAPI::GetFactoryInstance();
  extensions::BookmarksAPI::GetFactoryInstance();
  extensions::BookmarkManagerPrivateAPI::GetFactoryInstance();
  extensions::BrailleDisplayPrivateAPI::GetFactoryInstance();
  extensions::chromedirectsetting::ChromeDirectSettingAPI::GetFactoryInstance();
  extensions::CommandService::GetFactoryInstance();
  extensions::ContentSettingsService::GetFactoryInstance();
  extensions::CookiesAPI::GetFactoryInstance();
  extensions::DeveloperPrivateAPI::GetFactoryInstance();
  extensions::DialAPIFactory::GetInstance();
  extensions::EasyUnlockPrivateAPI::GetFactoryInstance();
  extensions::ExtensionActionAPI::GetFactoryInstance();
  extensions::ExtensionGarbageCollectorFactory::GetInstance();
  extensions::ExtensionStorageMonitorFactory::GetInstance();
  extensions::ExtensionSystemFactory::GetInstance();
  extensions::ExtensionWebUIOverrideRegistrar::GetFactoryInstance();
  extensions::FeedbackPrivateAPI::GetFactoryInstance();
  extensions::FontSettingsAPI::GetFactoryInstance();
  extensions::GcdPrivateAPI::GetFactoryInstance();
  extensions::HistoryAPI::GetFactoryInstance();
  extensions::HotwordPrivateEventService::GetFactoryInstance();
  extensions::IdentityAPI::GetFactoryInstance();
  extensions::InstallTrackerFactory::GetInstance();
  extensions::InstallVerifierFactory::GetInstance();
#if defined(OS_CHROMEOS)
  extensions::InputImeAPI::GetFactoryInstance();
  extensions::InputMethodAPI::GetFactoryInstance();
#elif defined(OS_LINUX) || defined(OS_WIN)
  extensions::InputImeAPI::GetFactoryInstance();
#endif
  extensions::LanguageSettingsPrivateDelegateFactory::GetInstance();
#if defined(OS_CHROMEOS)
  extensions::LogPrivateAPI::GetFactoryInstance();
#endif
  extensions::MDnsAPI::GetFactoryInstance();
#if defined(OS_CHROMEOS)
  extensions::MediaPlayerAPI::GetFactoryInstance();
#endif
  extensions::MenuManagerFactory::GetInstance();
  extensions::OmniboxAPI::GetFactoryInstance();
  extensions::PasswordsPrivateEventRouterFactory::GetInstance();
#if defined(ENABLE_PLUGINS)
  extensions::PluginManager::GetFactoryInstance();
#endif  // defined(ENABLE_PLUGINS)
  extensions::PreferenceAPI::GetFactoryInstance();
  extensions::ProcessesAPI::GetFactoryInstance();
  extensions::ScreenlockPrivateEventRouter::GetFactoryInstance();
  extensions::SessionsAPI::GetFactoryInstance();
  extensions::SettingsPrivateEventRouterFactory::GetInstance();
  extensions::SettingsOverridesAPI::GetFactoryInstance();
  extensions::SignedInDevicesManager::GetFactoryInstance();
#if defined(ENABLE_SPELLCHECK)
  extensions::SpellcheckAPI::GetFactoryInstance();
#endif
  extensions::StreamsPrivateAPI::GetFactoryInstance();
  extensions::TabCaptureRegistry::GetFactoryInstance();
  extensions::TabsWindowsAPI::GetFactoryInstance();
  extensions::TtsAPI::GetFactoryInstance();
   extensions::WarningBadgeServiceFactory::GetInstance();
   extensions::WebNavigationAPI::GetFactoryInstance();
   extensions::WebrtcAudioPrivateEventService::GetFactoryInstance();
 #if defined(OS_CHROMEOS)
   file_manager::EventRouterFactory::GetInstance();
 #endif
  TokenCacheServiceFactory::GetInstance();
  ToolbarActionsModelFactory::GetInstance();
  extensions::ExtensionGCMAppHandler::GetFactoryInstance();
}
