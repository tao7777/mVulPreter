static v8::Handle<v8::Value> strictFunctionCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.strictFunction");
     if (args.Length() < 3)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     ExceptionCode ec = 0;
     {
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
    EXCEPTION_BLOCK(float, a, static_cast<float>(MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined)->NumberValue()));
    EXCEPTION_BLOCK(int, b, V8int::HasInstance(MAYBE_MISSING_PARAMETER(args, 2, DefaultIsUndefined)) ? V8int::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 2, DefaultIsUndefined))) : 0);
    RefPtr<bool> result = imp->strictFunction(str, a, b, ec);
    if (UNLIKELY(ec))
        goto fail;
    return toV8(result.release(), args.GetIsolate());
    }
    fail:
    V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
