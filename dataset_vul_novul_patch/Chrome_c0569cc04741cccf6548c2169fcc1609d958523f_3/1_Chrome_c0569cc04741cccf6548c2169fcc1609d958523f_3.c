void Dispatcher::RegisterNativeHandlers(ModuleSystem* module_system,
                                        ScriptContext* context,
                                        Dispatcher* dispatcher,
                                        RequestSender* request_sender,
                                         V8SchemaRegistry* v8_schema_registry) {
   module_system->RegisterNativeHandler(
       "chrome", scoped_ptr<NativeHandler>(new ChromeNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "lazy_background_page",
      scoped_ptr<NativeHandler>(new LazyBackgroundPageNativeHandler(context)));
   module_system->RegisterNativeHandler(
       "logging", scoped_ptr<NativeHandler>(new LoggingNativeHandler(context)));
   module_system->RegisterNativeHandler("schema_registry",
                                        v8_schema_registry->AsNativeHandler());
  module_system->RegisterNativeHandler(
      "print", scoped_ptr<NativeHandler>(new PrintNativeHandler(context)));
   module_system->RegisterNativeHandler(
       "test_features",
       scoped_ptr<NativeHandler>(new TestFeaturesNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "test_native_handler",
      scoped_ptr<NativeHandler>(new TestNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "user_gestures",
      scoped_ptr<NativeHandler>(new UserGesturesNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "utils", scoped_ptr<NativeHandler>(new UtilsNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "v8_context",
      scoped_ptr<NativeHandler>(new V8ContextNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "event_natives", scoped_ptr<NativeHandler>(new EventBindings(context)));
  module_system->RegisterNativeHandler(
      "messaging_natives",
      scoped_ptr<NativeHandler>(MessagingBindings::Get(dispatcher, context)));
  module_system->RegisterNativeHandler(
      "apiDefinitions",
      scoped_ptr<NativeHandler>(
          new ApiDefinitionsNatives(dispatcher, context)));
  module_system->RegisterNativeHandler(
      "sendRequest",
      scoped_ptr<NativeHandler>(
          new SendRequestNatives(request_sender, context)));
  module_system->RegisterNativeHandler(
      "setIcon",
      scoped_ptr<NativeHandler>(new SetIconNatives(context)));
  module_system->RegisterNativeHandler(
      "activityLogger",
      scoped_ptr<NativeHandler>(new APIActivityLogger(context)));
  module_system->RegisterNativeHandler(
      "renderFrameObserverNatives",
      scoped_ptr<NativeHandler>(new RenderFrameObserverNatives(context)));

  module_system->RegisterNativeHandler(
      "file_system_natives",
      scoped_ptr<NativeHandler>(new FileSystemNatives(context)));

  module_system->RegisterNativeHandler(
      "app_window_natives",
      scoped_ptr<NativeHandler>(new AppWindowCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "blob_natives",
      scoped_ptr<NativeHandler>(new BlobNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "context_menus",
      scoped_ptr<NativeHandler>(new ContextMenusCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "css_natives", scoped_ptr<NativeHandler>(new CssNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "document_natives",
      scoped_ptr<NativeHandler>(new DocumentCustomBindings(context)));
  module_system->RegisterNativeHandler(
       "guest_view_internal",
       scoped_ptr<NativeHandler>(
           new GuestViewInternalCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "i18n", scoped_ptr<NativeHandler>(new I18NCustomBindings(context)));
   module_system->RegisterNativeHandler(
       "id_generator",
       scoped_ptr<NativeHandler>(new IdGeneratorCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "runtime", scoped_ptr<NativeHandler>(new RuntimeCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "display_source",
      scoped_ptr<NativeHandler>(new DisplaySourceCustomBindings(context)));
}
