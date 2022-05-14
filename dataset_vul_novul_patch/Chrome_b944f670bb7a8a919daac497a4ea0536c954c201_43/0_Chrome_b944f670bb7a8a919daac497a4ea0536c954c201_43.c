static EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionOverloadedMethod2(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     TestObj* objArg(toTestObj(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 1) {
        impl->overloadedMethod(objArg);
        return JSValue::encode(jsUndefined());
    }

    int intArg(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toInt32(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl->overloadedMethod(objArg, intArg);
    return JSValue::encode(jsUndefined());
}
