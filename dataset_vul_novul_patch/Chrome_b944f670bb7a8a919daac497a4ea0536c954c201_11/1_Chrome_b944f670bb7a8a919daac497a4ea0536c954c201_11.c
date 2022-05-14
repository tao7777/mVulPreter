EncodedJSValue JSC_HOST_CALL jsTestCustomNamedGetterPrototypeFunctionAnotherFunction(ExecState* exec)
{
    JSValue thisValue = exec->hostThisValue();
    if (!thisValue.inherits(&JSTestCustomNamedGetter::s_info))
        return throwVMTypeError(exec);
    JSTestCustomNamedGetter* castedThis = jsCast<JSTestCustomNamedGetter*>(asObject(thisValue));
     ASSERT_GC_OBJECT_INHERITS(castedThis, &JSTestCustomNamedGetter::s_info);
     TestCustomNamedGetter* impl = static_cast<TestCustomNamedGetter*>(castedThis->impl());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     const String& str(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    impl->anotherFunction(str);
    return JSValue::encode(jsUndefined());
}
