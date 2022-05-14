EncodedJSValue JSC_HOST_CALL JSTestSerializedScriptValueInterfaceConstructor::constructJSTestSerializedScriptValueInterface(ExecState* exec)
 {
     JSTestSerializedScriptValueInterfaceConstructor* castedThis = jsCast<JSTestSerializedScriptValueInterfaceConstructor*>(exec->callee());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     const String& hello(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    RefPtr<SerializedScriptValue> data(SerializedScriptValue::create(exec, MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    Array* transferList(toArray(MAYBE_MISSING_PARAMETER(exec, 2, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    RefPtr<TestSerializedScriptValueInterface> object = TestSerializedScriptValueInterface::create(hello, data, transferList);
    return JSValue::encode(asObject(toJS(exec, castedThis->globalObject(), object.get())));
}
