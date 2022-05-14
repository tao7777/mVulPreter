static v8::Handle<v8::Value> methodWithNonOptionalArgAndTwoOptionalArgsCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.methodWithNonOptionalArgAndTwoOptionalArgs");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
     TestObj* imp = V8TestObj::toNative(args.Holder());
     EXCEPTION_BLOCK(int, nonOpt, toInt32(MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined)));
     if (args.Length() <= 1) {
        imp->methodWithNonOptionalArgAndTwoOptionalArgs(nonOpt);
        return v8::Handle<v8::Value>();
    }
    EXCEPTION_BLOCK(int, opt1, toInt32(MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined)));
    if (args.Length() <= 2) {
        imp->methodWithNonOptionalArgAndTwoOptionalArgs(nonOpt, opt1);
        return v8::Handle<v8::Value>();
    }
    EXCEPTION_BLOCK(int, opt2, toInt32(MAYBE_MISSING_PARAMETER(args, 2, DefaultIsUndefined)));
    imp->methodWithNonOptionalArgAndTwoOptionalArgs(nonOpt, opt1, opt2);
    return v8::Handle<v8::Value>();
}
