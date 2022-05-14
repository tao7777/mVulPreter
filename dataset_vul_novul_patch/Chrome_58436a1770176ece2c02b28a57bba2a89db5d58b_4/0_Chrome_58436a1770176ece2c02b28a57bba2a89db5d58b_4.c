TestWebKitPlatformSupport::TestWebKitPlatformSupport(bool unit_test_mode)
      : unit_test_mode_(unit_test_mode) {
  v8::V8::SetCounterFunction(base::StatsTable::FindLocation);

  WebKit::initialize(this);
  WebKit::setLayoutTestMode(true);
  WebKit::WebSecurityPolicy::registerURLSchemeAsLocal(
       WebKit::WebString::fromUTF8("test-shell-resource"));
   WebKit::WebSecurityPolicy::registerURLSchemeAsNoAccess(
       WebKit::WebString::fromUTF8("test-shell-resource"));
  WebKit::WebSecurityPolicy::registerURLSchemeAsDisplayIsolated(
      WebKit::WebString::fromUTF8("test-shell-resource"));
  WebKit::WebSecurityPolicy::registerURLSchemeAsEmptyDocument(
      WebKit::WebString::fromUTF8("test-shell-resource"));
   WebScriptController::enableV8SingleThreadMode();
   WebKit::WebRuntimeFeatures::enableSockets(true);
   WebKit::WebRuntimeFeatures::enableApplicationCache(true);
  WebKit::WebRuntimeFeatures::enableDatabase(true);
  WebKit::WebRuntimeFeatures::enableDataTransferItems(true);
  WebKit::WebRuntimeFeatures::enablePushState(true);
  WebKit::WebRuntimeFeatures::enableNotifications(true);
  WebKit::WebRuntimeFeatures::enableTouch(true);
  WebKit::WebRuntimeFeatures::enableGamepad(true);

  bool enable_media = false;
  FilePath module_path;
  if (PathService::Get(base::DIR_MODULE, &module_path)) {
#if defined(OS_MACOSX)
    if (base::mac::AmIBundled())
      module_path = module_path.DirName().DirName().DirName();
#endif
    if (media::InitializeMediaLibrary(module_path))
      enable_media = true;
  }
  WebKit::WebRuntimeFeatures::enableMediaPlayer(enable_media);
  LOG_IF(WARNING, !enable_media) << "Failed to initialize the media library.\n";

  WebKit::WebRuntimeFeatures::enableGeolocation(false);

  if (!appcache_dir_.CreateUniqueTempDir()) {
    LOG(WARNING) << "Failed to create a temp dir for the appcache, "
                    "using in-memory storage.";
    DCHECK(appcache_dir_.path().empty());
  }
  SimpleAppCacheSystem::InitializeOnUIThread(appcache_dir_.path());

  WebKit::WebDatabase::setObserver(&database_system_);

  blob_registry_ = new TestShellWebBlobRegistryImpl();

  file_utilities_.set_sandbox_enabled(false);

  if (!file_system_root_.CreateUniqueTempDir()) {
    LOG(WARNING) << "Failed to create a temp dir for the filesystem."
                    "FileSystem feature will be disabled.";
    DCHECK(file_system_root_.path().empty());
  }

#if defined(OS_WIN)
  SetThemeEngine(NULL);
#endif

  net::HttpCache::Mode cache_mode = net::HttpCache::NORMAL;
  net::CookieMonster::EnableFileScheme();

  SimpleResourceLoaderBridge::Init(FilePath(), cache_mode, true);

  webkit_glue::SetJavaScriptFlags(" --expose-gc");
  WebScriptController::registerExtension(extensions_v8::GCExtension::Get());
}
