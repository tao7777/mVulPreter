v8::Local<v8::Value> V8ValueConverterImpl::ToV8Array(
    v8::Isolate* isolate,
    v8::Local<v8::Object> creation_context,
     const base::ListValue* val) const {
   v8::Local<v8::Array> result(v8::Array::New(isolate, val->GetSize()));
 
   for (size_t i = 0; i < val->GetSize(); ++i) {
     const base::Value* child = NULL;
     CHECK(val->Get(i, &child));

    v8::Local<v8::Value> child_v8 =
         ToV8ValueImpl(isolate, creation_context, child);
     CHECK(!child_v8.IsEmpty());
 
    v8::TryCatch try_catch(isolate);
    result->Set(static_cast<uint32_t>(i), child_v8);
    if (try_catch.HasCaught())
      LOG(ERROR) << "Setter for index " << i << " threw an exception.";
   }
 
   return result;
}
