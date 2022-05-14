 static EncodedJSValue JSC_HOST_CALL jsTestObjConstructorFunctionOverloadedMethod12(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     const String& type(ustringToString(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).isEmpty() ? UString() : MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toString(exec)->value(exec)));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    TestObj::overloadedMethod1(type);
    return JSValue::encode(jsUndefined());
}
