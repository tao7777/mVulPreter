 static void locationReplaceableAttributeSetter(v8::Local<v8::Value> jsValue, const v8::PropertyCallbackInfo<void>& info)
 {
     TestObjectPython* proxyImp = V8TestObjectPython::toNative(info.Holder());
    RefPtr<TestNode> imp = WTF::getPtr(proxyImp->locationReplaceable());
     if (!imp)
         return;
     V8TRYCATCH_FOR_V8STRINGRESOURCE_VOID(V8StringResource<>, cppValue, jsValue);
    imp->setHref(cppValue);
}
