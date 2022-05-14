EncodedJSValue JSC_HOST_CALL jsTestObjPrototypeFunctionAddEventListener(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestObj::s_info))
        return throwVMTypeError(exec);
    JSTestObj* castedThis = jsCast<JSTestObj*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestObj::s_info);
     TestObj* impl = static_cast<TestObj*>(castedThis->impl());
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     JSValue listener = exec->argument(1);
     if (!listener.isObject())
         return JSValue::encode(jsUndefined());
    impl->addEventListener(ustringToAtomicString(exec->argument(0).toString(exec)->value(exec)), JSEventListener::create(asObject(listener), castedThis, false, currentWorld(exec)), exec->argument(2).toBoolean(exec));
    return JSValue::encode(jsUndefined());
}
