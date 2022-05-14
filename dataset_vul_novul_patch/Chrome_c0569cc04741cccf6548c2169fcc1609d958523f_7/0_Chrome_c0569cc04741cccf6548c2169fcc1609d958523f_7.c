void ObjectBackedNativeHandler::Router(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Object> data = args.Data().As<v8::Object>();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::Value> handler_function_value;
  v8::Local<v8::Value> feature_name_value;
  if (!GetPrivate(context, data, kHandlerFunction, &handler_function_value) ||
      handler_function_value->IsUndefined() ||
      !GetPrivate(context, data, kFeatureName, &feature_name_value) ||
      !feature_name_value->IsString()) {
    ScriptContext* script_context =
        ScriptContextSet::GetContextByV8Context(context);
    console::Error(script_context ? script_context->GetRenderFrame() : nullptr,
                   "Extension view no longer exists");
    return;
  }

  if (content::WorkerThread::GetCurrentId() == 0) {
    ScriptContext* script_context =
        ScriptContextSet::GetContextByV8Context(context);
    v8::Local<v8::String> feature_name_string =
        feature_name_value->ToString(context).ToLocalChecked();
     std::string feature_name = *v8::String::Utf8Value(feature_name_string);
    if (script_context && !feature_name.empty()) {
      Feature::Availability availability =
          script_context->GetAvailability(feature_name);
      if (!availability.is_available()) {
        DVLOG(1) << feature_name
                 << " is not available: " << availability.message();
        return;
      }
     }
   }
  CHECK(handler_function_value->IsExternal());
  static_cast<HandlerFunction*>(
      handler_function_value.As<v8::External>()->Value())->Run(args);

  v8::ReturnValue<v8::Value> ret = args.GetReturnValue();
  v8::Local<v8::Value> ret_value = ret.Get();
  if (ret_value->IsObject() && !ret_value->IsNull() &&
      !ContextCanAccessObject(context, v8::Local<v8::Object>::Cast(ret_value),
                              true)) {
    NOTREACHED() << "Insecure return value";
    ret.SetUndefined();
  }
}
