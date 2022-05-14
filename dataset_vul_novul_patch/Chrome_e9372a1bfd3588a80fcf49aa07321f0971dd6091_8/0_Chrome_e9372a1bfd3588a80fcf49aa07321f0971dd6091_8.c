static v8::Handle<v8::Value> methodCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestMediaQueryListListener.method");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestMediaQueryListListener* imp = V8TestMediaQueryListListener::toNative(args.Holder());
     EXCEPTION_BLOCK(RefPtr<MediaQueryListListener>, listener, MediaQueryListListener::create(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     imp->method(listener);
    return v8::Handle<v8::Value>();
}
