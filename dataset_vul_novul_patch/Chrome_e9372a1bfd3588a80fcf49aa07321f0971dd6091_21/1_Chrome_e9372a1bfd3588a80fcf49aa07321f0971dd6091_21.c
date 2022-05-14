static v8::Handle<v8::Value> methodThatRequiresAllArgsAndThrowsCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.methodThatRequiresAllArgsAndThrows");
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     ExceptionCode ec = 0;
     {
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, strArg, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
    EXCEPTION_BLOCK(TestObj*, objArg, V8TestObj::HasInstance(MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined)) ? V8TestObj::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined))) : 0);
    RefPtr<TestObj> result = imp->methodThatRequiresAllArgsAndThrows(strArg, objArg, ec);
    if (UNLIKELY(ec))
        goto fail;
    return toV8(result.release(), args.GetIsolate());
    }
    fail:
    V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
