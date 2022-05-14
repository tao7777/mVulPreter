void ObjectBackedNativeHandler::RouteFunction(
    const std::string& name,
    const std::string& feature_name,
    const HandlerFunction& handler_function) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(context_->v8_context());

   v8::Local<v8::Object> data = v8::Object::New(isolate);
   SetPrivate(data, kHandlerFunction,
              v8::External::New(isolate, new HandlerFunction(handler_function)));
  DCHECK(feature_name.empty() ||
         ExtensionAPI::GetSharedInstance()->GetFeatureDependency(feature_name))
      << feature_name;
   SetPrivate(data, kFeatureName,
              v8_helpers::ToV8StringUnsafe(isolate, feature_name));
   v8::Local<v8::FunctionTemplate> function_template =
      v8::FunctionTemplate::New(isolate, Router, data);
  v8::Local<v8::ObjectTemplate>::New(isolate, object_template_)
      ->Set(isolate, name.c_str(), function_template);
  router_data_.Append(data);
}
