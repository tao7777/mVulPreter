static v8::Handle<v8::Value> objMethodWithArgsCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.objMethodWithArgs");
     if (args.Length() < 3)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(int, intArg, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, strArg, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    EXCEPTION_BLOCK(TestObj*, objArg, V8TestObj::HasInstance(MAYBE_MISSING_PARAMETER(args, 2, DefaultIsUndefined)) ? V8TestObj::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 2, DefaultIsUndefined))) : 0);
    return toV8(imp->objMethodWithArgs(intArg, strArg, objArg), args.GetIsolate());
}
