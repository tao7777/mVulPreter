static v8::Handle<v8::Value> overloadedMethod4Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod4");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(int, intArg, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     imp->overloadedMethod(intArg);
    return v8::Handle<v8::Value>();
}
