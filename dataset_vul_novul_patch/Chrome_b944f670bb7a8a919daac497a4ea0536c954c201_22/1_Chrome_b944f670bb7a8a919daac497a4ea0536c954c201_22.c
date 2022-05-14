 static EncodedJSValue JSC_HOST_CALL jsTestObjConstructorFunctionOverloadedMethod11(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     int arg(MAYBE_MISSING_PARAMETER(exec, 0, DefaultIsUndefined).toInt32(exec));
     if (exec->hadException())
         return JSValue::encode(jsUndefined());
    TestObj::overloadedMethod1(arg);
    return JSValue::encode(jsUndefined());
}
