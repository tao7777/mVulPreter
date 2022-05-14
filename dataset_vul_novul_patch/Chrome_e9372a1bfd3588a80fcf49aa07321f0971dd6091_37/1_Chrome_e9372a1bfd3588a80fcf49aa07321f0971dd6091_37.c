static v8::Handle<v8::Value> overloadedMethod7Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod7");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(RefPtr<DOMStringList>, arrayArg, v8ValueToWebCoreDOMStringList(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     imp->overloadedMethod(arrayArg);
    return v8::Handle<v8::Value>();
}
