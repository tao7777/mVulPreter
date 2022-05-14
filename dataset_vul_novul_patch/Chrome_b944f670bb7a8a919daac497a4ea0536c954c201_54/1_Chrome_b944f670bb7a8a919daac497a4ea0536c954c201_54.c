EncodedJSValue JSC_HOST_CALL jsTestSerializedScriptValueInterfacePrototypeFunctionAcceptTransferList(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestSerializedScriptValueInterface::s_info))
        return throwVMTypeError(exec);
    JSTestSerializedScriptValueInterface* castedThis = jsCast<JSTestSerializedScriptValueInterface*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestSerializedScriptValueInterface::s_info);
     TestSerializedScriptValueInterface* impl = static_cast<TestSerializedScriptValueInterface*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     RefPtr<SerializedScriptValue> data(SerializedScriptValue::create(exec, MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 1) {
        impl->acceptTransferList(data);
        return JSValue::encode(jsUndefined());
    }

    Array* transferList(toArray(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl->acceptTransferList(data, transferList);
    return JSValue::encode(jsUndefined());
}
