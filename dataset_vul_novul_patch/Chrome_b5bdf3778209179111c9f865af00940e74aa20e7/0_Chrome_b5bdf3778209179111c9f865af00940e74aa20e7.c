v8::Local<v8::Value> V8ValueConverterImpl::ToV8Array(
    v8::Isolate* isolate,
    v8::Local<v8::Object> creation_context,
     const base::ListValue* val) const {
   v8::Local<v8::Array> result(v8::Array::New(isolate, val->GetSize()));
 
  // TODO(robwu): Callers should pass in the context.
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

   for (size_t i = 0; i < val->GetSize(); ++i) {
     const base::Value* child = NULL;
     CHECK(val->Get(i, &child));

    v8::Local<v8::Value> child_v8 =
         ToV8ValueImpl(isolate, creation_context, child);
     CHECK(!child_v8.IsEmpty());
 
    v8::Maybe<bool> maybe =
        result->CreateDataProperty(context, static_cast<uint32_t>(i), child_v8);
    if (!maybe.IsJust() || !maybe.FromJust())
      LOG(ERROR) << "Failed to set value at index " << i;
   }
 
   return result;
}
