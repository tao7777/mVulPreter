EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionStrictFunction(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 3)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     ExceptionCode ec = 0;
     const String& str(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
        return JSValue::encode(jsUndefined());
    float a(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toFloat(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    if (exec->argumentCount() > 2 && !exec->argument(2).isUndefinedOrNull() && !exec->argument(2).inherits(&JSint::s_info))
        return throwVMTypeError(exec);
    int* b(toint(MAYBE_MISSING_PARAMETER(exec, 2, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->strictFunction(str, a, b, ec)));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}
