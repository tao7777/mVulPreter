void V8ContextNativeHandler::RunWithNativesEnabledModuleSystem(
     const v8::FunctionCallbackInfo<v8::Value>& args) {
   CHECK_EQ(args.Length(), 1);
   CHECK(args[0]->IsFunction());
  v8::Local<v8::Value> call_with_args[] = {
    context()->module_system()->NewInstance()};
   ModuleSystem::NativesEnabledScope natives_enabled(context()->module_system());
  context()->CallFunction(v8::Local<v8::Function>::Cast(args[0]), 1,
                          call_with_args);
 }
