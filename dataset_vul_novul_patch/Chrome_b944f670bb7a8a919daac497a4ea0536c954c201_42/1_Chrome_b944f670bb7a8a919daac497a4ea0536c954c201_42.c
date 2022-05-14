static EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionOverloadedMethod1(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     TestObj* objArg(toTestObj(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    const String& strArg(ustringToString(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toString(exec)->value(exec)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl->overloadedMethod(objArg, strArg);
    return JSValue::encode(jsUndefined());
}
