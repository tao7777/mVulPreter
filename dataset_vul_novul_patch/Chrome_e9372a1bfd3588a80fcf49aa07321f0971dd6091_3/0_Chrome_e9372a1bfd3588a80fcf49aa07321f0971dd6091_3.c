v8::Handle<v8::Value> V8TestEventConstructor::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.TestEventConstructor.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

    if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
 
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
 
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, type, args[0]);
     TestEventConstructorInit eventInit;
    if (args.Length() >= 2) {
        EXCEPTION_BLOCK(Dictionary, options, args[1]);
        if (!fillTestEventConstructorInit(eventInit, options))
            return v8::Undefined();
    }

    RefPtr<TestEventConstructor> event = TestEventConstructor::create(type, eventInit);

    V8DOMWrapper::setDOMWrapper(args.Holder(), &info, event.get());
    V8DOMWrapper::setJSWrapperForDOMObject(event.release(), v8::Persistent<v8::Object>::New(args.Holder()), args.GetIsolate());
    return args.Holder();
}
