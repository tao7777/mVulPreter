static v8::Handle<v8::Value> convert2Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.convert2");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(b*, , V8b::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8b::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     imp->convert2();
    return v8::Handle<v8::Value>();
}
