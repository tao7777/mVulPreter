static v8::Handle<v8::Value> methodWithNonCallbackArgAndCallbackArgCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.methodWithNonCallbackArgAndCallbackArg");
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(int, nonCallback, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     if (args.Length() <= 1 || !args[1]->IsFunction())
        return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());
    RefPtr<TestCallback> callback = V8TestCallback::create(args[1], getScriptExecutionContext());
    imp->methodWithNonCallbackArgAndCallbackArg(nonCallback, callback);
    return v8::Handle<v8::Value>();
}
