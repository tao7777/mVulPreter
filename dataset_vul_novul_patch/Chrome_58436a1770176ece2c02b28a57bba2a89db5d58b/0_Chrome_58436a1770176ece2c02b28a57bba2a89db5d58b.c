void RenderThreadImpl::EnsureWebKitInitialized() {
  if (webkit_platform_support_.get())
    return;

  v8::V8::SetCounterFunction(base::StatsTable::FindLocation);
  v8::V8::SetCreateHistogramFunction(CreateHistogram);
  v8::V8::SetAddHistogramSampleFunction(AddHistogramSample);

  webkit_platform_support_.reset(new RendererWebKitPlatformSupportImpl);
  WebKit::initialize(webkit_platform_support_.get());

  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableThreadedCompositing)) {
    compositor_thread_.reset(new CompositorThread(this));
    AddFilter(compositor_thread_->GetMessageFilter());
    WebKit::WebCompositor::initialize(compositor_thread_->GetWebThread());
  } else
    WebKit::WebCompositor::initialize(NULL);
  compositor_initialized_ = true;
 
   WebScriptController::enableV8SingleThreadMode();
 
  RenderThreadImpl::RegisterSchemes();

   const CommandLine& command_line = *CommandLine::ForCurrentProcess();
 
   webkit_glue::EnableWebCoreLogChannels(
      command_line.GetSwitchValueASCII(switches::kWebCoreLogChannels));

  if (command_line.HasSwitch(switches::kPlaybackMode) ||
      command_line.HasSwitch(switches::kRecordMode) ||
      command_line.HasSwitch(switches::kNoJsRandomness)) {
    RegisterExtension(extensions_v8::PlaybackExtension::Get());
  }

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDomAutomationController)) {
    base::StringPiece extension = content::GetContentClient()->GetDataResource(
        IDR_DOM_AUTOMATION_JS);
    RegisterExtension(new v8::Extension(
        "dom_automation.js", extension.data(), 0, NULL, extension.size()));
  }

  web_database_observer_impl_.reset(
      new WebDatabaseObserverImpl(sync_message_filter()));
  WebKit::WebDatabase::setObserver(web_database_observer_impl_.get());

  WebRuntimeFeatures::enableSockets(
      !command_line.HasSwitch(switches::kDisableWebSockets));

  WebRuntimeFeatures::enableDatabase(
      !command_line.HasSwitch(switches::kDisableDatabases));

  WebRuntimeFeatures::enableDataTransferItems(
      !command_line.HasSwitch(switches::kDisableDataTransferItems));

  WebRuntimeFeatures::enableApplicationCache(
      !command_line.HasSwitch(switches::kDisableApplicationCache));

  WebRuntimeFeatures::enableNotifications(
      !command_line.HasSwitch(switches::kDisableDesktopNotifications));

  WebRuntimeFeatures::enableLocalStorage(
      !command_line.HasSwitch(switches::kDisableLocalStorage));
  WebRuntimeFeatures::enableSessionStorage(
      !command_line.HasSwitch(switches::kDisableSessionStorage));

  WebRuntimeFeatures::enableIndexedDatabase(true);

  WebRuntimeFeatures::enableGeolocation(
      !command_line.HasSwitch(switches::kDisableGeolocation));

  WebKit::WebRuntimeFeatures::enableMediaSource(
      command_line.HasSwitch(switches::kEnableMediaSource));

  WebRuntimeFeatures::enableMediaPlayer(
      media::IsMediaLibraryInitialized());

  WebKit::WebRuntimeFeatures::enableMediaStream(
      command_line.HasSwitch(switches::kEnableMediaStream));

  WebKit::WebRuntimeFeatures::enableFullScreenAPI(
      !command_line.HasSwitch(switches::kDisableFullScreen));

  WebKit::WebRuntimeFeatures::enablePointerLock(
      command_line.HasSwitch(switches::kEnablePointerLock));

  WebKit::WebRuntimeFeatures::enableVideoTrack(
      command_line.HasSwitch(switches::kEnableVideoTrack));

#if defined(OS_CHROMEOS)
  WebRuntimeFeatures::enableWebAudio(false);
#else
  WebRuntimeFeatures::enableWebAudio(
      !command_line.HasSwitch(switches::kDisableWebAudio));
#endif

  WebRuntimeFeatures::enablePushState(true);

  WebRuntimeFeatures::enableTouch(
      command_line.HasSwitch(switches::kEnableTouchEvents));

  WebRuntimeFeatures::enableDeviceMotion(
      command_line.HasSwitch(switches::kEnableDeviceMotion));

  WebRuntimeFeatures::enableDeviceOrientation(
      !command_line.HasSwitch(switches::kDisableDeviceOrientation));

  WebRuntimeFeatures::enableSpeechInput(
      !command_line.HasSwitch(switches::kDisableSpeechInput));

  WebRuntimeFeatures::enableScriptedSpeech(
      command_line.HasSwitch(switches::kEnableScriptedSpeech));

  WebRuntimeFeatures::enableFileSystem(
      !command_line.HasSwitch(switches::kDisableFileSystem));

  WebRuntimeFeatures::enableJavaScriptI18NAPI(
      !command_line.HasSwitch(switches::kDisableJavaScriptI18NAPI));

  WebRuntimeFeatures::enableGamepad(
      command_line.HasSwitch(switches::kEnableGamepad));

  WebRuntimeFeatures::enableQuota(true);

  WebRuntimeFeatures::enableShadowDOM(
      command_line.HasSwitch(switches::kEnableShadowDOM));

  WebRuntimeFeatures::enableStyleScoped(
      command_line.HasSwitch(switches::kEnableStyleScoped));

  FOR_EACH_OBSERVER(RenderProcessObserver, observers_, WebKitInitialized());

  if (content::GetContentClient()->renderer()->
         RunIdleHandlerWhenWidgetsHidden()) {
    ScheduleIdleHandler(kLongIdleHandlerDelayMs);
   }
 }
