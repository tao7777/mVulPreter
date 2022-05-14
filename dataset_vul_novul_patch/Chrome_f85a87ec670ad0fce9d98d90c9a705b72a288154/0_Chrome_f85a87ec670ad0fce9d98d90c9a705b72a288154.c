 static void locationAttributeSetter(v8::Local<v8::Value> jsValue, const v8::PropertyCallbackInfo<void>& info)
 {
     TestObject* proxyImp = V8TestObject::toNative(info.Holder());
    RefPtr<TestNode> imp = WTF::getPtr(proxyImp->location());
     if (!imp)
         return;
     V8TRYCATCH_FOR_V8STRINGRESOURCE_VOID(V8StringResource<>, cppValue, jsValue);
    imp->setHref(cppValue);
}
