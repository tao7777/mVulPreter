 EncodedJSValue JSC_HOST_CALL jsTestObjConstructorFunctionClassMethod2(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
     return JSValue::encode(JSTestObj::classMethod2(exec));
 }
