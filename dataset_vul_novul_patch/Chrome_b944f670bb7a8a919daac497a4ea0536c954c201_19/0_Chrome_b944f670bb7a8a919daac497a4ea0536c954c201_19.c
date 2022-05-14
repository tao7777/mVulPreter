EncodedJSValue JSC_HOST_CALL JSTestNamedConstructorNamedConstructor::constructJSTestNamedConstructor(ExecState* exec)
 {
     JSTestNamedConstructorNamedConstructor* castedThis = jsCast<JSTestNamedConstructorNamedConstructor*>(exec->callee());
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     ExceptionCode ec = 0;
     const String& str1(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
        return JSValue::encode(jsUndefined());
    const String& str2(ustringToString(MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 1, DefaultIsUndefined).toString(exec)->value(exec)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    const String& str3(ustringToString(MAYBE_MISSING_PARAMETER(exec, 2, DefaultIsNullString).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 2, DefaultIsNullString).toString(exec)->value(exec)));
    if (exec->hadException())
        return JSValue::encode(jsUndefined());
    RefPtr<TestNamedConstructor> object = TestNamedConstructor::createForJSConstructor(castedThis->document(), str1, str2, str3, ec);
    if (ec) {
        setDOMException(exec, ec);
        return JSValue::encode(JSValue());
    }
    return JSValue::encode(asObject(toJS(exec, castedThis->globalObject(), object.get())));
}
