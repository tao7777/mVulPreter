EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionDispatchEvent(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestEventTarget::s_info))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestEventTarget::s_info);
     TestEventTarget* impl = static_cast<TestEventTarget*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     ExceptionCode ec = 0;
     Event* evt(toEvent(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = jsBoolean(impl->dispatchEvent(evt, ec));
    setDOMException(exec, ec);
    return JSValue::encode(result);
}
