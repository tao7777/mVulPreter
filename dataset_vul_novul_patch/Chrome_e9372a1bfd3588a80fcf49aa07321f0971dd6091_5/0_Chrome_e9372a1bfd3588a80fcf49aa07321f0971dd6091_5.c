static v8::Handle<v8::Value> itemCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestEventTarget.item");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestEventTarget* imp = V8TestEventTarget::toNative(args.Holder());
     ExceptionCode ec = 0;
     {
    EXCEPTION_BLOCK(int, index, toUInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
    if (UNLIKELY(index < 0)) {
        ec = INDEX_SIZE_ERR;
        goto fail;
    }
    return toV8(imp->item(index), args.GetIsolate());
    }
    fail:
    V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
