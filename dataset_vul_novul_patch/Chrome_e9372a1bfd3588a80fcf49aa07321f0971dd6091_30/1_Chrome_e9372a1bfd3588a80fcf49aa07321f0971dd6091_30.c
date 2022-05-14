static v8::Handle<v8::Value> overloadedMethod12Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod12");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, type, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     TestObj::overloadedMethod1(type);
     return v8::Handle<v8::Value>();
}
