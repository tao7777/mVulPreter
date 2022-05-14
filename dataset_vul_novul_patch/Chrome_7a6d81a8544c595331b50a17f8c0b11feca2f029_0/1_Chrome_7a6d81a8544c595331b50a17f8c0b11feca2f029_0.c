 static void reflectStringAttributeAttributeSetter(v8::Local<v8::Value> v8Value, const v8::FunctionCallbackInfo<v8::Value>& info)
 {
     v8::Local<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toImpl(holder);
     V8StringResource<> cppValue = v8Value;
     if (!cppValue.prepare())
         return;
    impl->setAttribute(HTMLNames::reflectstringattributeAttr, cppValue);
}
