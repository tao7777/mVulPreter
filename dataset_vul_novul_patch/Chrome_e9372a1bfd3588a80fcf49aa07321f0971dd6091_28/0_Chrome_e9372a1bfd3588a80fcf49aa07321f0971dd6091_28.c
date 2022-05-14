static v8::Handle<v8::Value> optionsObjectCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.optionsObject");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(Dictionary, oo, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     if (args.Length() > 0 && !oo.isUndefinedOrNull() && !oo.isObject()) {
        return V8Proxy::throwTypeError("Not an object.");
    }
    if (args.Length() <= 1) {
        imp->optionsObject(oo);
        return v8::Handle<v8::Value>();
    }
    EXCEPTION_BLOCK(Dictionary, ooo, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    if (args.Length() > 1 && !ooo.isUndefinedOrNull() && !ooo.isObject()) {
        return V8Proxy::throwTypeError("Not an object.");
    }
    imp->optionsObject(oo, ooo);
    return v8::Handle<v8::Value>();
}
