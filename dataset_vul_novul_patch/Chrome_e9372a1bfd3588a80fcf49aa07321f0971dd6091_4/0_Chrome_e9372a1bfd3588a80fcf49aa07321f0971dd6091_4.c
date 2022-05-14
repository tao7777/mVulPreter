static v8::Handle<v8::Value> dispatchEventCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestEventTarget.dispatchEvent");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestEventTarget* imp = V8TestEventTarget::toNative(args.Holder());
     ExceptionCode ec = 0;
     {
    EXCEPTION_BLOCK(Event*, evt, V8Event::HasInstance(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)) ? V8Event::toNative(v8::Handle<v8::Object>::Cast(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined))) : 0);
    bool result = imp->dispatchEvent(evt, ec);
    if (UNLIKELY(ec))
        goto fail;
    return v8Boolean(result);
    }
    fail:
    V8Proxy::setDOMException(ec, args.GetIsolate());
    return v8::Handle<v8::Value>();
}
