static v8::Handle<v8::Value> convert5Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.convert5");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(e*, , V8e::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8e::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     imp->convert5();
    return v8::Handle<v8::Value>();
}
