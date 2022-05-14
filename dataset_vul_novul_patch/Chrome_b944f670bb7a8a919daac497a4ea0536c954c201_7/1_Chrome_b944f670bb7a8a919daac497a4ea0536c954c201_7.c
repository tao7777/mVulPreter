EncodedJSValue JSC_HOST_CALL JSWorkerConstructor::constructJSWorker(ExecState* exec)
{
     JSWorkerConstructor* jsConstructor = jsCast<JSWorkerConstructor*>(exec->callee());
 
     if (!exec->argumentCount())
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
 
     UString scriptURL = exec->argument(0).toString(exec)->value(exec);
     if (exec->hadException())
        return JSValue::encode(JSValue());

    DOMWindow* window = asJSDOMWindow(exec->lexicalGlobalObject())->impl();

    ExceptionCode ec = 0;
    RefPtr<Worker> worker = Worker::create(window->document(), ustringToString(scriptURL), ec);
    if (ec) {
        setDOMException(exec, ec);
        return JSValue::encode(JSValue());
    }

    return JSValue::encode(asObject(toJS(exec, jsConstructor->globalObject(), worker.release())));
}
