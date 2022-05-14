static v8::Handle<v8::Value> V8TestNamedConstructorConstructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.TestNamedConstructor.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

    if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
        return args.Holder();

    Frame* frame = V8Proxy::retrieveFrameForCurrentContext();
    if (!frame)
        return V8Proxy::throwError(V8Proxy::ReferenceError, "TestNamedConstructor constructor associated frame is unavailable", args.GetIsolate());

    Document* document = frame->document();

     toV8(document, args.GetIsolate());
 
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError();
 
     ExceptionCode ec = 0;
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str1, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str2, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, str3, MAYBE_MISSING_PARAMETER(args, 2, DefaultIsNullString));

    RefPtr<TestNamedConstructor> impl = TestNamedConstructor::createForJSConstructor(document, str1, str2, str3, ec);
    v8::Handle<v8::Object> wrapper = args.Holder();
    if (ec)
        goto fail;

    V8DOMWrapper::setDOMWrapper(wrapper, &V8TestNamedConstructorConstructor::info, impl.get());
    V8DOMWrapper::setJSWrapperForActiveDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper), args.GetIsolate());
    return args.Holder();
  fail:
    return throwError(ec, args.GetIsolate());
}
