v8::Handle<v8::Value> V8TestSerializedScriptValueInterface::constructorCallback(const v8::Arguments& args)
{
    INC_STATS("DOM.TestSerializedScriptValueInterface.Constructor");

    if (!args.IsConstructCall())
        return V8Proxy::throwTypeError("DOM object constructor cannot be called as a function.");

     if (ConstructorMode::current() == ConstructorMode::WrapExistingObject)
         return args.Holder();
     if (args.Length() < 2)
        return V8Proxy::throwNotEnoughArgumentsError();
     STRING_TO_V8PARAMETER_EXCEPTION_BLOCK(V8Parameter<>, hello, MAYBE_MISSING_PARAMETER(args, 0, DefaultIsUndefined));
     MessagePortArray messagePortArrayTransferList;
     ArrayBufferArray arrayBufferArrayTransferList;
    if (args.Length() > 2) {
        if (!extractTransferables(args[2], messagePortArrayTransferList, arrayBufferArrayTransferList))
            return V8Proxy::throwTypeError("Could not extract transferables");
    }
    bool dataDidThrow = false;
    RefPtr<SerializedScriptValue> data = SerializedScriptValue::create(args[1], &messagePortArrayTransferList, &arrayBufferArrayTransferList, dataDidThrow, args.GetIsolate());
    if (dataDidThrow)
        return v8::Undefined();

    RefPtr<TestSerializedScriptValueInterface> impl = TestSerializedScriptValueInterface::create(hello, data, messagePortArrayTransferList);
    v8::Handle<v8::Object> wrapper = args.Holder();

    V8DOMWrapper::setDOMWrapper(wrapper, &info, impl.get());
    V8DOMWrapper::setJSWrapperForDOMObject(impl.release(), v8::Persistent<v8::Object>::New(wrapper), args.GetIsolate());
    return args.Holder();
}
