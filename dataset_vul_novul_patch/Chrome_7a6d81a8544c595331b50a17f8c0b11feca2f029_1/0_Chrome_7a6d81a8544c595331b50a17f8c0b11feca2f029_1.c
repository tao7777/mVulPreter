 static void reflectUrlStringAttributeAttributeSetter(v8::Local<v8::Value> v8Value, const v8::FunctionCallbackInfo<v8::Value>& info)
 {
     v8::Local<v8::Object> holder = info.Holder();
    TestInterfaceNode* impl = V8TestInterfaceNode::toImpl(holder);
     V8StringResource<> cppValue = v8Value;
     if (!cppValue.prepare())
         return;
    impl->setAttribute(HTMLNames::reflecturlstringattributeAttr, cppValue);
}
