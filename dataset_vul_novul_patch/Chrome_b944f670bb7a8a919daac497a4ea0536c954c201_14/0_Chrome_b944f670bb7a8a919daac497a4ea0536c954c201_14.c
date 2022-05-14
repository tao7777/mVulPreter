EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionItem(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestEventTarget::s_info))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestEventTarget::s_info);
     TestEventTarget* impl = static_cast<TestEventTarget*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     int index(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toUInt32(exec));
     if (index < 0) {
         setDOMException(exec, INDEX_SIZE_ERR);
        return JSValue::encode(jsUndefined());
    }
    if (exec->hadException())
        return JSValue::encode(jsUndefined());

    JSC::JSValue result = toJS(exec, castedThis->globalObject(), WTF::getPtr(impl->item(index)));
    return JSValue::encode(result);
}
