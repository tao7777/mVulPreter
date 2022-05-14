void ProfileDependencyManager::AssertFactoriesBuilt() {
  if (built_factories_)
    return;

#if defined(ENABLE_BACKGROUND)
  BackgroundContentsServiceFactory::GetInstance();
#endif
  BookmarkModelFactory::GetInstance();
#if defined(ENABLE_CAPTIVE_PORTAL_DETECTION)
  captive_portal::CaptivePortalServiceFactory::GetInstance();
#endif
  ChromeURLDataManagerFactory::GetInstance();
#if defined(ENABLE_PRINTING)
  CloudPrintProxyServiceFactory::GetInstance();
#endif
  CookieSettings::Factory::GetInstance();
#if defined(ENABLE_NOTIFICATIONS)
  DesktopNotificationServiceFactory::GetInstance();
#endif
  DownloadServiceFactory::GetInstance();
#if defined(ENABLE_EXTENSIONS)
  extensions::AppRestoreServiceFactory::GetInstance();
   extensions::BluetoothAPIFactory::GetInstance();
   extensions::CommandServiceFactory::GetInstance();
   extensions::CookiesAPIFactory::GetInstance();
  extensions::DialAPIFactory::GetInstance();
   extensions::ExtensionSystemFactory::GetInstance();
   extensions::FontSettingsAPIFactory::GetInstance();
   extensions::IdleManagerFactory::GetInstance();
  extensions::ManagedModeAPIFactory::GetInstance();
  extensions::ProcessesAPIFactory::GetInstance();
  extensions::SuggestedLinksRegistryFactory::GetInstance();
  extensions::TabCaptureRegistryFactory::GetInstance();
  extensions::WebNavigationAPIFactory::GetInstance();
  ExtensionManagementAPIFactory::GetInstance();
#endif
  FaviconServiceFactory::GetInstance();
  FindBarStateFactory::GetInstance();
#if defined(USE_AURA)
  GesturePrefsObserverFactoryAura::GetInstance();
#endif
  GlobalErrorServiceFactory::GetInstance();
  GoogleURLTrackerFactory::GetInstance();
  HistoryServiceFactory::GetInstance();
  MediaGalleriesPreferencesFactory::GetInstance();
  NTPResourceCacheFactory::GetInstance();
  PasswordStoreFactory::GetInstance();
  PersonalDataManagerFactory::GetInstance();
#if !defined(OS_ANDROID)
  PinnedTabServiceFactory::GetInstance();
#endif
  PluginPrefsFactory::GetInstance();
#if defined(ENABLE_CONFIGURATION_POLICY) && !defined(OS_CHROMEOS)
  policy::UserPolicySigninServiceFactory::GetInstance();
#endif
  predictors::AutocompleteActionPredictorFactory::GetInstance();
  predictors::PredictorDatabaseFactory::GetInstance();
  predictors::ResourcePrefetchPredictorFactory::GetInstance();
  prerender::PrerenderManagerFactory::GetInstance();
  prerender::PrerenderLinkManagerFactory::GetInstance();
  ProfileSyncServiceFactory::GetInstance();
  ProtocolHandlerRegistryFactory::GetInstance();
#if defined(ENABLE_PROTECTOR_SERVICE)
  protector::ProtectorServiceFactory::GetInstance();
#endif
#if defined(ENABLE_SESSION_SERVICE)
  SessionServiceFactory::GetInstance();
#endif
  ShortcutsBackendFactory::GetInstance();
  ThumbnailServiceFactory::GetInstance();
  SigninManagerFactory::GetInstance();
#if defined(ENABLE_INPUT_SPEECH)
  SpeechInputExtensionManager::InitializeFactory();
  ChromeSpeechRecognitionPreferences::InitializeFactory();
#endif
  SpellcheckServiceFactory::GetInstance();
  TabRestoreServiceFactory::GetInstance();
  TemplateURLFetcherFactory::GetInstance();
  TemplateURLServiceFactory::GetInstance();
#if defined(ENABLE_THEMES)
  ThemeServiceFactory::GetInstance();
#endif
  TokenServiceFactory::GetInstance();
  UserStyleSheetWatcherFactory::GetInstance();
  VisitedLinkMasterFactory::GetInstance();
  WebDataServiceFactory::GetInstance();
#if defined(ENABLE_WEB_INTENTS)
  WebIntentsRegistryFactory::GetInstance();
#endif

  built_factories_ = true;
}
