static v8::Handle<v8::Value> overloadedMethod3Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod3");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, strArg, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     imp->overloadedMethod(strArg);
    return v8::Handle<v8::Value>();
}
