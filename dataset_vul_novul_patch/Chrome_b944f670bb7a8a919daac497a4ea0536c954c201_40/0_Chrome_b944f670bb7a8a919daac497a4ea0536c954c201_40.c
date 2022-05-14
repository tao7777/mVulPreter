EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionObjMethodWithArgs(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 3)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     int intArg(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toInt32(exec));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    const String& strArg(ustringToString(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toString(exec)->value(exec)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    TestObj* objArg(toTestObj(MAYBE_MISSING_PARAMETER(exec, 2, DefaultIsUndefined)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->objMethodWithArgs(intArg, strArg, objArg)));
    return JSValue::encode(result);
}
