v8::Handle<v8::Value> V8TestObj::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.TestObj.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

     if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     if (args.Length() <= 0 || !args[0]->IsFunction())
         return throwError(TYPE_MISMATCH_ERR, args.GetIsolate());
     RefPtr<TestCallback> testCallback = V8TestCallback::create(args[0], getScriptExecutionContext());

    RefPtr<TestObj> impl = TestObj::create(testCallback);
    v8::Handle<v8::Object> wrapper = args.Holder();

    V8DOMWrapper::setDOMWrapper(wrapper, &info, impl.get());
    V8DOMWrapper::setJSWrapperForDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper), args.GetIsolate());
    return args.Holder();
}
