void ChromeExtensionsDispatcherDelegate::RegisterNativeHandlers(
    extensions::Dispatcher* dispatcher,
    extensions::ModuleSystem* module_system,
    extensions::ScriptContext* context) {
  module_system->RegisterNativeHandler(
      "app", std::unique_ptr<NativeHandler>(
                 new extensions::AppBindings(dispatcher, context)));
  module_system->RegisterNativeHandler(
      "sync_file_system",
      std::unique_ptr<NativeHandler>(
          new extensions::SyncFileSystemCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "file_browser_handler",
      std::unique_ptr<NativeHandler>(
          new extensions::FileBrowserHandlerCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "file_manager_private",
      std::unique_ptr<NativeHandler>(
          new extensions::FileManagerPrivateCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "notifications_private",
      std::unique_ptr<NativeHandler>(
          new extensions::NotificationsNativeHandler(context)));
  module_system->RegisterNativeHandler(
      "mediaGalleries",
      std::unique_ptr<NativeHandler>(
          new extensions::MediaGalleriesCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "page_capture", std::unique_ptr<NativeHandler>(
                          new extensions::PageCaptureCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "platform_keys_natives",
      std::unique_ptr<NativeHandler>(
          new extensions::PlatformKeysNatives(context)));
  module_system->RegisterNativeHandler(
      "tabs", std::unique_ptr<NativeHandler>(
                  new extensions::TabsCustomBindings(context)));
  module_system->RegisterNativeHandler(
      "webstore", std::unique_ptr<NativeHandler>(
                      new extensions::WebstoreBindings(context)));
#if defined(ENABLE_WEBRTC)
  module_system->RegisterNativeHandler(
      "cast_streaming_natives",
      std::unique_ptr<NativeHandler>(
          new extensions::CastStreamingNativeHandler(context)));
#endif
  module_system->RegisterNativeHandler(
       "automationInternal",
       std::unique_ptr<NativeHandler>(
           new extensions::AutomationInternalCustomBindings(context)));
 }
