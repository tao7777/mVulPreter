void V8ContextNativeHandler::RunWithNativesEnabledModuleSystem(
void V8ContextNativeHandler::RunWithNativesEnabled(
     const v8::FunctionCallbackInfo<v8::Value>& args) {
   CHECK_EQ(args.Length(), 1);
   CHECK(args[0]->IsFunction());
   ModuleSystem::NativesEnabledScope natives_enabled(context()->module_system());
  context()->CallFunction(v8::Local<v8::Function>::Cast(args[0]));
 }
