 EncodedJSValue JSC_HOST_CALL jsTestObjConstructorFunctionClassMethod2(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
     return JSValue::encode(JSTestObj::classMethod2(exec));
 }
