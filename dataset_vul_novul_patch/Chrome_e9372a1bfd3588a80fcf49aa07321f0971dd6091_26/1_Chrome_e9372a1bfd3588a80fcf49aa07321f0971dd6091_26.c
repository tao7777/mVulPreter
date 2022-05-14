static v8::Handle<v8::Value> methodWithSequenceArgCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.methodWithSequenceArg");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(sequence<ScriptProfile>*, sequenceArg, toNativeArray<ScriptProfile>(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     imp->methodWithSequenceArg(sequenceArg);
    return v8::Handle<v8::Value>();
}
