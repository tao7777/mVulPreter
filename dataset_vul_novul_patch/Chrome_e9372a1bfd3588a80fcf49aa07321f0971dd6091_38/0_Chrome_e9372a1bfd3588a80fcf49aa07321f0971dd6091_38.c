static v8::Handle<v8::Value> serializedValueCallback(const v8::Arguments& args)
 {
     INC_STATS("DOM.TestObj.serializedValue");
     if (args.Length() < 1)
        return V8Proxy::throwNotEnoughArgumentsError(args.GetIsolate());
     TestObj* imp = V8TestObj::toNative(args.Holder());
     bool serializedArgDidThrow = false;
     RefPtr<SerializedScriptValue> serializedArg = SerializedScriptValue::create(args[0], 0, 0, serializedArgDidThrow, args.GetIsolate());
    if (serializedArgDidThrow)
        return v8::Undefined();
    imp->serializedValue(serializedArg);
    return v8::Handle<v8::Value>();
}
