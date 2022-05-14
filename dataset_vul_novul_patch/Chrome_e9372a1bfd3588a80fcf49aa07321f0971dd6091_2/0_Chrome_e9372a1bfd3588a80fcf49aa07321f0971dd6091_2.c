static v8::Handle<v8::Value> anotherFunctionCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestCustomNamedGetter.anotherFunction");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestCustomNamedGetter* imp = V8TestCustomNamedGetter::toNative(args.Holder());
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     imp->anotherFunction(str);
    return v8::Handle<v8::Value>();
}
