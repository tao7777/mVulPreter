EncodedJSValue JSC_HOST_CALL JSSharedWorkerConstructor::constructJSSharedWorker(ExecState* exec)
{
     JSSharedWorkerConstructor* jsConstructor = jsCast<JSSharedWorkerConstructor*>(exec->callee());
 
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
 
     UString scriptURL = exec->argument(0).toString(exec)->value(exec);
     UString name;
    if (exec->argumentCount() > 1)
        name = exec->argument(1).toString(exec)->value(exec);

    if (exec->hadException())
        return JSValue::encode(JSValue());

    DOMWindow* window = asJSDOMWindow(exec->lexicalGlobalObject())->impl();
    ExceptionCode ec = 0;
    RefPtr<SharedWorker> worker = SharedWorker::create(window->document(), ustringToString(scriptURL), ustringToString(name), ec);
    if (ec) {
        setDOMException(exec, ec);
        return JSValue::encode(JSValue());
    }

    return JSValue::encode(asObject(toJS(exec, jsConstructor->globalObject(), worker.release())));
}
