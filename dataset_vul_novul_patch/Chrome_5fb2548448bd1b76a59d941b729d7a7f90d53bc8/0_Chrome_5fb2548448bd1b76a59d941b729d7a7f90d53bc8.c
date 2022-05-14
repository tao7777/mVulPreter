v8::Local<v8::Object> V8SchemaRegistry::GetSchema(const std::string& api) {
  if (schema_cache_ != NULL) {
    v8::Local<v8::Object> cached_schema = schema_cache_->Get(api);
    if (!cached_schema.IsEmpty()) {
      return cached_schema;
    }
  }


  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = GetOrCreateContext(isolate);
  v8::Context::Scope context_scope(context);

  const base::DictionaryValue* schema =
      ExtensionAPI::GetSharedInstance()->GetSchema(api);
  CHECK(schema) << api;
  std::unique_ptr<V8ValueConverter> v8_value_converter(
      V8ValueConverter::create());
  v8::Local<v8::Value> value = v8_value_converter->ToV8Value(schema, context);
   CHECK(!value.IsEmpty());
 
   v8::Local<v8::Object> v8_schema(v8::Local<v8::Object>::Cast(value));
  DeepFreeze(v8_schema, context);
   schema_cache_->Set(api, v8_schema);
 
   return handle_scope.Escape(v8_schema);
}
