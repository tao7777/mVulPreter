v8::Handle<v8::Value> V8Intent::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.Intent.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

     if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     if (args.Length() == 1) {
        EXCEPTION_BLOCK(Dictionary, options, args[0]);
        ExceptionCode ec = 0;
        RefPtr<Intent> impl = Intent::create(ScriptState::current(), options, ec);
        if (ec)
            return throwError(ec, args.GetIsolate());

        v8::Handle<v8::Object> wrapper = args.Holder();
        V8DOMWrapper::setDOMWrapper(wrapper, &info, impl.get());
        V8DOMWrapper::setJSWrapperForDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper));
        return wrapper;
    }

    ExceptionCode ec = 0;
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, action, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
    STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, type, MAYBE_MISSING_PARAMETER(args, 1, DefaultIsUndefined));
    MessagePortArray messagePortArrayTransferList;
    ArrayBufferArray arrayBufferArrayTransferList;
    if (args.Length() > 3) {
        if (!extractTransferables(args[3], messagePortArrayTransferList, arrayBufferArrayTransferList))
            return V8Proxy::throwTypeError("Could not extract transferables");
    }
    bool dataDidThrow = false;
    RefPtr<SerializedScriptValue> data = SerializedScriptValue::create(args[2], &messagePortArrayTransferList, &arrayBufferArrayTransferList, dataDidThrow);
    if (dataDidThrow)
        return throwError(DATA_CLONE_ERR, args.GetIsolate());

    RefPtr<Intent> impl = Intent::create(action, type, data, messagePortArrayTransferList, ec);
    if (ec)
        return throwError(ec, args.GetIsolate());

    v8::Handle<v8::Object> wrapper = args.Holder();
    V8DOMWrapper::setDOMWrapper(wrapper, &info, impl.get());
    V8DOMWrapper::setJSWrapperForDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper));
    return wrapper;
}
