EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionSerializedValue(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     RefPtr<SerializedScriptValue> serializedArg(SerializedScriptValue::create(exec, MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    impl->serializedValue(serializedArg);
    return JSValue::encode(jsUndefined());
}
