static v8::Handle<v8::Value> methodReturningSequenceCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.methodReturningSequence");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(int, intArg, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     return v8Array(imp->methodReturningSequence(intArg), args.GetIsolate());
}
