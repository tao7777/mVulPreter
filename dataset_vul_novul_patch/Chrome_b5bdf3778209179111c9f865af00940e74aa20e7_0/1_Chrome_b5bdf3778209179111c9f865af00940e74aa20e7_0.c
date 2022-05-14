v8::Local<v8::Value> V8ValueConverterImpl::ToV8Object(
    v8::Isolate* isolate,
    v8::Local<v8::Object> creation_context,
     const base::DictionaryValue* val) const {
   v8::Local<v8::Object> result(v8::Object::New(isolate));
 
   for (base::DictionaryValue::Iterator iter(*val);
        !iter.IsAtEnd(); iter.Advance()) {
     const std::string& key = iter.key();
     v8::Local<v8::Value> child_v8 =
         ToV8ValueImpl(isolate, creation_context, &iter.value());
     CHECK(!child_v8.IsEmpty());
 
    v8::TryCatch try_catch(isolate);
    result->Set(
        v8::String::NewFromUtf8(
            isolate, key.c_str(), v8::String::kNormalString, key.length()),
         child_v8);
    if (try_catch.HasCaught()) {
      LOG(ERROR) << "Setter for property " << key.c_str() << " threw an "
                 << "exception.";
    }
   }
 
   return result;
}
