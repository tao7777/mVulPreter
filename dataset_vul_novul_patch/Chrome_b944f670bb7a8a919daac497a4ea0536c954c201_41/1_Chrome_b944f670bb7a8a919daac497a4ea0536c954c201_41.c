EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionOptionsObject(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     Dictionary oo(exec, MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 1) {
        impl->optionsObject(oo);
        return JSValue::encode(jsUndefined());
    }

    Dictionary ooo(exec, MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl->optionsObject(oo, ooo);
    return JSValue::encode(jsUndefined());
}
