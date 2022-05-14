EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionMethodThatRequiresAllArgsAndThrows(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     ExceptionCode ec = 0;
     const String& strArg(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
        return JSValue::encode(jsUndefined());
    TestObj* objArg(toTestObj(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->methodThatRequiresAllArgsAndThrows(strArg, objArg, ec)));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}
