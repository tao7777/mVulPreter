static v8::Handle<v8::Value> convert3Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.convert3");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(c*, , V8c::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8c::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     imp->convert3();
    return v8::Handle<v8::Value>();
}
