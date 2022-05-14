static v8::Handle<v8::Value> overloadedMethod2Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod2");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(TestObj*, objArg, V8TestObj::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8TestObj::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     if (args.Length() <= 1) {
        imp->overloadedMethod(objArg);
        return v8::Handle<v8::Value>();
    }
    EXCEPTION_BLOCK(int, intArg, toInt32(MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined)));
    imp->overloadedMethod(objArg, intArg);
    return v8::Handle<v8::Value>();
}
