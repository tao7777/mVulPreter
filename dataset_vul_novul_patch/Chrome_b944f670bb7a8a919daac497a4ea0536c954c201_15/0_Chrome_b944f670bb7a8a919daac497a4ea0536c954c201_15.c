EncodedJSValue JSC_HOST_CALL jsTestEventTargetPrototypeFunctionRemoveEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestEventTarget::s_info))
        return throwVMTypeError(exec);
    JSTestEventTarget* castedThis = jsCast<JSTestEventTarget*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestEventTarget::s_info);
     TestEventTarget* impl = static_cast<TestEventTarget*>(castedThis->impl());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     JSValue listener = exec->argument(1);
     if (!listener.isObject())
         return JSValue::encode(jsUndefined());
    impl->removeEventListener(ustringToAtomicString(exec->argument(0).toString(exec)->value(exec)), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)).get(), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}
