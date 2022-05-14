EncodedJSValue JSC_HOST_CALL JSTestInterfaceConstructor::constructJSTestInterface(ExecState* exec)
 {
     JSTestInterfaceConstructor* castedThis = jsCast<JSTestInterfaceConstructor*>(exec->callee());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     ExceptionCode ec = 0;
     const String& str1(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
        return JSValue::encode(jsUndefined());
    const String& str2(ustringToString(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toString(exec)->value(exec)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    ScriptExecutionContext* context = castedThis->scriptExecutionContext();
    if (!context)
        return throwVMError(exec, createReferenceError(exec, "TestInterface constructor associated document is unavailable"));
    RefPtr<TestInterface> object = TestInterface::create(context, str1, str2, ec);
    if (ec) {
        setDOMException(exec, ec);
        return JSValue::encode(JSValue());
    }
    return JSValue::encode(asObject(toJS(exec, castedThis->globalObject(), object.get())));
}
