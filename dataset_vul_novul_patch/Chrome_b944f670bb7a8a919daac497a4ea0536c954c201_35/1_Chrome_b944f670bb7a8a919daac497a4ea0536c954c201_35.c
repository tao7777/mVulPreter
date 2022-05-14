EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionMethodWithNonCallbackArgAndCallbackArg(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     int nonCallback(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toInt32(exec));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    if (exec->argumentCount() <= 1 || !exec->argument(1).isFunction()) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }
    RefPtr<TestCallback> callback = JSTestCallback::create(asObject(exec->argument(1)), castedThis->globalObject());
    impl->methodWithNonCallbackArgAndCallbackArg(nonCallback, callback);
    return JSValue::encode(jsUndefined());
}
