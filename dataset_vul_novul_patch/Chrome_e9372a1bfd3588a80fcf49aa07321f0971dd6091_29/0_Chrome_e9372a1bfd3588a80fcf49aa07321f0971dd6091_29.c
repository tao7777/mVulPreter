static v8::Handle<v8::Value> overloadedMethod11Callback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.overloadedMethod11");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     EXCEPTION_BLOCK(int, arg, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     TestObj::overloadedMethod1(arg);
     return v8::Handle<v8::Value>();
}
