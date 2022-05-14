static v8::Handle<v8::Value> postMessageCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestActiveDOMObject.postMessage");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestActiveDOMObject* imp = V8TestActiveDOMObject::toNative(args.Holder());
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, message, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     imp->postMessage(message);
    return v8::Handle<v8::Value>();
}
