 static void reflectStringAttributeAttributeGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
 {
     v8::Local<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toImpl(holder);
     v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::reflectstringattributeAttr), info.GetIsolate());
 }
