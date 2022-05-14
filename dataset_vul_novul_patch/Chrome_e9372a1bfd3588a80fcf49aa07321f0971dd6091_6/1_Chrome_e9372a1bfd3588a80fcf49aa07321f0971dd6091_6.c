v8::Handle<v8::Value> V8TestInterface::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.TestInterface.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

     if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     ExceptionCode ec = 0;
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str1, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str2, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));

    ScriptExecutionContext* context = getScriptExecutionContext();
    if (!context)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "TestInterface constructor's associated context is not available", args.GetIsolate());

    RefPtr<TestInterface> impl = TestInterface::create(context, str1, str2, ec);
    v8::Handle<v8::Object> wrapper = args.Holder();
    if (ec)
        goto fail;

    V8DOMWrapper::setDOMWrapper(wrapper, &info, impl.get());
    V8DOMWrapper::setJSWrapperForActiveDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper), args.GetIsolate());
    return args.Holder();
  fail:
    return throwError(ec, args.GetIsolate());
}
