EncodedJSValue JSC_HOST_CALL jsTestActiveDOMObjectPrototypeFunctionExcitingFunction(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestActiveDOMObject::s_info))
        return throwVMTypeError(exec);
    JSTestActiveDOMObject* castedThis = jsCast<JSTestActiveDOMObject*>(asObject(thisValue));
    ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestActiveDOMObject::s_info);
    if (!castedThis->allowsAccessFrom(exec))
         return JSValue::encode(jsUndefined());
     TestActiveDOMObject* impl = static_cast<TestActiveDOMObject*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     Node* nextChild(toNode(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    impl->excitingFunction(nextChild);
    return JSValue::encode(jsUndefined());
}
