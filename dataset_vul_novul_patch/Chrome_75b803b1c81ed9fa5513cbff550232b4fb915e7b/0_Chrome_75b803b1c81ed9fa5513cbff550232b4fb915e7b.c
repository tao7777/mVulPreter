v8::Local<v8::Value> ModuleSystem::RequireForJsInner(
    v8::Local<v8::String> module_name) {
  v8::EscapableHandleScope handle_scope(GetIsolate());
  v8::Local<v8::Context> v8_context = context()->v8_context();
  v8::Context::Scope context_scope(v8_context);

  v8::Local<v8::Object> global(context()->v8_context()->Global());

  v8::Local<v8::Value> modules_value;
  if (!GetPrivate(global, kModulesField, &modules_value) ||
      modules_value->IsUndefined()) {
    Warn(GetIsolate(), "Extension view no longer exists");
    return v8::Undefined(GetIsolate());
  }
 
   v8::Local<v8::Object> modules(v8::Local<v8::Object>::Cast(modules_value));
   v8::Local<v8::Value> exports;
  if (!GetPrivateProperty(v8_context, modules, module_name, &exports) ||
       !exports->IsUndefined())
     return handle_scope.Escape(exports);
 
   exports = LoadModule(*v8::String::Utf8Value(module_name));
  SetPrivateProperty(v8_context, modules, module_name, exports);
   return handle_scope.Escape(exports);
 }
