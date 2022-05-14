void ApiTestEnvironment::RegisterModules() {
  v8_schema_registry_.reset(new V8SchemaRegistry);
  const std::vector<std::pair<std::string, int> > resources =
      Dispatcher::GetJsResources();
  for (std::vector<std::pair<std::string, int> >::const_iterator resource =
           resources.begin();
       resource != resources.end();
       ++resource) {
    if (resource->first != "test_environment_specific_bindings")
      env()->RegisterModule(resource->first, resource->second);
  }
  Dispatcher::RegisterNativeHandlers(env()->module_system(),
                                     env()->context(),
                                     NULL,
                                     NULL,
                                     v8_schema_registry_.get());
  env()->module_system()->RegisterNativeHandler(
      "process",
      scoped_ptr<NativeHandler>(new ProcessInfoNativeHandler(
          env()->context(),
          env()->context()->GetExtensionID(),
          env()->context()->GetContextTypeDescription(),
          false,
          false,
          2,
          false)));
  env()->RegisterTestFile("test_environment_specific_bindings",
                           "unit_test_environment_specific_bindings.js");
 
   env()->OverrideNativeHandler("activityLogger",
                               "exports.$set('LogAPICall', function() {});");
   env()->OverrideNativeHandler(
       "apiDefinitions",
      "exports.$set('GetExtensionAPIDefinitionsForTest',"
                    "function() { return [] });");
   env()->OverrideNativeHandler(
       "event_natives",
      "exports.$set('AttachEvent', function() {});"
      "exports.$set('DetachEvent', function() {});"
      "exports.$set('AttachFilteredEvent', function() {});"
      "exports.$set('AttachFilteredEvent', function() {});"
      "exports.$set('MatchAgainstEventFilter', function() { return [] });");
 
   gin::ModuleRegistry::From(env()->context()->v8_context())
       ->AddBuiltinModule(env()->isolate(),
                         mojo::js::Core::kModuleName,
                         mojo::js::Core::GetModule(env()->isolate()));
  gin::ModuleRegistry::From(env()->context()->v8_context())
      ->AddBuiltinModule(env()->isolate(),
                         mojo::js::Support::kModuleName,
                         mojo::js::Support::GetModule(env()->isolate()));
  gin::Handle<TestServiceProvider> service_provider =
      TestServiceProvider::Create(env()->isolate());
  service_provider_ = service_provider.get();
  gin::ModuleRegistry::From(env()->context()->v8_context())
      ->AddBuiltinModule(env()->isolate(),
                         "content/public/renderer/service_provider",
                         service_provider.ToV8());
}
