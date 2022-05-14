static v8::Handle<v8::Value> overloadedMethod1Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod1");
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(TestObj*, objArg, V8TestObj::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8TestObj::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, strArg, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    imp->overloadedMethod(objArg, strArg);
    return v8::Handle<v8::Value>();
}
