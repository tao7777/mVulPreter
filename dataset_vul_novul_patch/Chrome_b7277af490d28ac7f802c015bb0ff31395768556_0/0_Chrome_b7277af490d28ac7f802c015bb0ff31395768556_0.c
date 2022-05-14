static void LongOrNullAttributeAttributeSetter(
    v8::Local<v8::Value> v8_value, const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  ALLOW_UNUSED_LOCAL(isolate);

  v8::Local<v8::Object> holder = info.Holder();
  ALLOW_UNUSED_LOCAL(holder);

  TestObject* impl = V8TestObject::ToImpl(holder);
 
   ExceptionState exception_state(isolate, ExceptionState::kSetterContext, "TestObject", "longOrNullAttribute");
 
  bool is_null = IsUndefinedOrNull(v8_value);

  int32_t cpp_value = is_null ? int32_t() : NativeValueTraits<IDLLong>::NativeValue(info.GetIsolate(), v8_value, exception_state);
   if (exception_state.HadException())
     return;
 
   impl->setLongOrNullAttribute(cpp_value, is_null);
 }
