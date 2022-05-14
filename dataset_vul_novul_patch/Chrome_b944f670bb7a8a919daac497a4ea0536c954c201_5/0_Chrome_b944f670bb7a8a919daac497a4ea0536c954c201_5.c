 EncodedJSValue JSC_HOST_CALL JSWebKitMutationObserverConstructor::constructJSWebKitMutationObserver(ExecState* exec)
 {
     if (exec->argumentCount() < 1)
        return throwVMError(exec, createNotEnoughArgumentsError(exec));
 
     JSObject* object = exec->argument(0).getObject();
     if (!object) {
        setDOMException(exec, TYPE_MISMATCH_ERR);
        return JSValue::encode(jsUndefined());
    }

    JSWebKitMutationObserverConstructor* jsConstructor = jsCast<JSWebKitMutationObserverConstructor*>(exec->callee());
    RefPtr<MutationCallback> callback = JSMutationCallback::create(object, jsConstructor->globalObject());
    return JSValue::encode(asObject(toJS(exec, jsConstructor->globalObject(), WebKitMutationObserver::create(callback.release()))));
}
