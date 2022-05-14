EncodedJSValue JSC_HOST_CALL jsTestMediaQueryListListenerPrototypeFunctionMethod(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestMediaQueryListListener::s_info))
        return throwVMTypeError(exec);
    JSTestMediaQueryListListener* castedThis = jsCast<JSTestMediaQueryListListener*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestMediaQueryListListener::s_info);
     TestMediaQueryListListener* impl = static_cast<TestMediaQueryListListener*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     RefPtr<MediaQueryListListener> listener(MediaQueryListListener::create(ScriptValue(exec->globalData(), MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined))));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    impl->method(listener);
    return JSValue::encode(jsUndefined());
}
