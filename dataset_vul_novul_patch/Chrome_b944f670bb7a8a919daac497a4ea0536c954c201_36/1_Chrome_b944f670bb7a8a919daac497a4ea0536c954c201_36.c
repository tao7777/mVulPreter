EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionMethodWithNonOptionalArgAndOptionalArg(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     int nonOpt(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toInt32(exec));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());

    size_t argsCount = exec->argumentCount();
    if (argsCount <= 1) {
        impl->methodWithNonOptionalArgAndOptionalArg(nonOpt);
        return JSValue::encode(jsUndefined());
    }

    int opt(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toInt32(exec));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    impl->methodWithNonOptionalArgAndOptionalArg(nonOpt, opt);
    return JSValue::encode(jsUndefined());
}
