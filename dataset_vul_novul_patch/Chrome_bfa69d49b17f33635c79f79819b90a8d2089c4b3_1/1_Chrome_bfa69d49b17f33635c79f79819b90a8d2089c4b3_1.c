void RenderThread::EnsureWebKitInitialized() {
  if (webkit_client_.get())
    return;

  v8::V8::SetCounterFunction(StatsTable::FindLocation);
  v8::V8::SetCreateHistogramFunction(CreateHistogram);
  v8::V8::SetAddHistogramSampleFunction(AddHistogramSample);

  webkit_client_.reset(new RendererWebKitClientImpl);
  WebKit::initialize(webkit_client_.get());

  WebScriptController::enableV8SingleThreadMode();

  WebString chrome_ui_scheme(ASCIIToUTF16(chrome::kChromeUIScheme));
  WebSecurityPolicy::registerURLSchemeAsLocal(chrome_ui_scheme);
  WebSecurityPolicy::registerURLSchemeAsNoAccess(chrome_ui_scheme);

  WebString print_ui_scheme(ASCIIToUTF16(chrome::kPrintScheme));
  WebSecurityPolicy::registerURLSchemeAsLocal(print_ui_scheme);
  WebSecurityPolicy::registerURLSchemeAsNoAccess(print_ui_scheme);

#if defined(OS_WIN)
  WebScriptController::registerExtension(extensions_v8::GearsExtension::Get());
#endif
  WebScriptController::registerExtension(
      extensions_v8::IntervalExtension::Get());
  WebScriptController::registerExtension(
      extensions_v8::LoadTimesExtension::Get());
  WebScriptController::registerExtension(
      extensions_v8::ExternalExtension::Get());

  const WebKit::WebString kExtensionScheme =
      WebKit::WebString::fromUTF8(chrome::kExtensionScheme);

  WebScriptController::registerExtension(
      ExtensionProcessBindings::Get(), kExtensionScheme);

  WebScriptController::registerExtension(
      BaseJsV8Extension::Get(), EXTENSION_GROUP_CONTENT_SCRIPTS);
  WebScriptController::registerExtension(
      BaseJsV8Extension::Get(), kExtensionScheme);
  WebScriptController::registerExtension(
      JsonSchemaJsV8Extension::Get(), EXTENSION_GROUP_CONTENT_SCRIPTS);
  WebScriptController::registerExtension(JsonSchemaJsV8Extension::Get(),
                                         kExtensionScheme);
  WebScriptController::registerExtension(
      EventBindings::Get(), EXTENSION_GROUP_CONTENT_SCRIPTS);
  WebScriptController::registerExtension(EventBindings::Get(),
                                         kExtensionScheme);
  WebScriptController::registerExtension(
      RendererExtensionBindings::Get(), EXTENSION_GROUP_CONTENT_SCRIPTS);
  WebScriptController::registerExtension(
      RendererExtensionBindings::Get(), kExtensionScheme);
  WebScriptController::registerExtension(
      ExtensionApiTestV8Extension::Get(), kExtensionScheme);
  WebScriptController::registerExtension(
      ExtensionApiTestV8Extension::Get(), EXTENSION_GROUP_CONTENT_SCRIPTS);

  renderer_web_database_observer_.reset(new RendererWebDatabaseObserver(this));
  WebKit::WebDatabase::setObserver(renderer_web_database_observer_.get());

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();

  if (command_line.HasSwitch(switches::kEnableBenchmarking)) {
    WebScriptController::registerExtension(
        extensions_v8::BenchmarkingExtension::Get());
  }

  if (command_line.HasSwitch(switches::kPlaybackMode) ||
      command_line.HasSwitch(switches::kRecordMode) ||
      command_line.HasSwitch(switches::kNoJsRandomness)) {
    WebScriptController::registerExtension(
        extensions_v8::PlaybackExtension::Get());
  }

  WebRuntimeFeatures::enableMediaPlayer(
      RenderProcess::current()->initialized_media_library());

  WebRuntimeFeatures::enableSockets(
      command_line.HasSwitch(switches::kEnableWebSockets));
 
   WebRuntimeFeatures::enableDatabase(
       command_line.HasSwitch(switches::kEnableDatabases));
 }
