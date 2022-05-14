v8::Local<v8::Value> ModuleSystem::LoadModule(const std::string& module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> v8_context = context()->v8_context();
  v8::Context::Scope context_scope(v8_context);

  v8::Local<v8::Value> source(GetSource(module_name));
  if (source.IsEmpty() || source->IsUndefined()) {
    Fatal(context_, "No source for require(" + module_name + ")");
    return v8::Undefined(GetIsolate());
  }
  v8::Local<v8::String> wrapped_source(
      WrapSource(v8::Local<v8::String>::Cast(source)));
  v8::Local<v8::String> v8_module_name;
  if (!ToV8String(GetIsolate(), module_name.c_str(), &v8_module_name)) {
    NOTREACHED() << "module_name is too long";
    return v8::Undefined(GetIsolate());
  }
  v8::Local<v8::Value> func_as_value =
      RunString(wrapped_source, v8_module_name);
  if (func_as_value.IsEmpty() || func_as_value->IsUndefined()) {
    Fatal(context_, "Bad source for require(" + module_name + ")");
    return v8::Undefined(GetIsolate());
  }

  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(func_as_value);

   v8::Local<v8::Object> define_object = v8::Object::New(GetIsolate());
   gin::ModuleRegistry::InstallGlobals(GetIsolate(), define_object);
 
  v8::Local<v8::Value> exports = v8::Object::New(GetIsolate());
   v8::Local<v8::Object> natives(NewInstance());
   CHECK(!natives.IsEmpty());  // this can fail if v8 has issues
 
  v8::Local<v8::Value> args[] = {
      GetPropertyUnsafe(v8_context, define_object, "define"),
      GetPropertyUnsafe(v8_context, natives, "require",
                        v8::NewStringType::kInternalized),
      GetPropertyUnsafe(v8_context, natives, "requireNative",
                        v8::NewStringType::kInternalized),
      GetPropertyUnsafe(v8_context, natives, "requireAsync",
                        v8::NewStringType::kInternalized),
      exports,
      console::AsV8Object(GetIsolate()),
      GetPropertyUnsafe(v8_context, natives, "privates",
                        v8::NewStringType::kInternalized),
      context_->safe_builtins()->GetArray(),
      context_->safe_builtins()->GetFunction(),
      context_->safe_builtins()->GetJSON(),
      context_->safe_builtins()->GetObjekt(),
      context_->safe_builtins()->GetRegExp(),
      context_->safe_builtins()->GetString(),
      context_->safe_builtins()->GetError(),
  };
  {
    v8::TryCatch try_catch(GetIsolate());
    try_catch.SetCaptureMessage(true);
    context_->CallFunction(func, arraysize(args), args);
    if (try_catch.HasCaught()) {
      HandleException(try_catch);
      return v8::Undefined(GetIsolate());
    }
  }
  return handle_scope.Escape(exports);
}
