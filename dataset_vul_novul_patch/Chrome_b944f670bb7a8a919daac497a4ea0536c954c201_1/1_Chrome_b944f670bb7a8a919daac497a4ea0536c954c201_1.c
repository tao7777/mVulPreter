EncodedJSValue JSC_HOST_CALL JSDeprecatedPeerConnectionConstructor::constructJSDeprecatedPeerConnection(ExecState* exec)
{
    JSDeprecatedPeerConnectionConstructor* jsConstructor = static_cast<JSDeprecatedPeerConnectionConstructor*>(exec->callee());
    ScriptExecutionContext* context = jsConstructor->scriptExecutionContext();
    if (!context)
         return throwVMError(exec, createReferenceError(exec, "DeprecatedPeerConnection constructor associated document is unavailable"));
 
     if (exec->argumentCount() < 2)
        return throwVMError(exec, createTypeError(exec, "Not enough arguments"));
 
     String serverConfiguration = ustringToString(exec->argument(0).toString(exec)->value(exec));
     if (exec->hadException())
        return JSValue::encode(JSValue());

    RefPtr<SignalingCallback> signalingCallback = createFunctionOnlyCallback<JSSignalingCallback>(exec, static_cast<JSDOMGlobalObject*>(exec->lexicalGlobalObject()), exec->argument(1));
    if (exec->hadException())
        return JSValue::encode(JSValue());

    RefPtr<DeprecatedPeerConnection> peerConnection = DeprecatedPeerConnection::create(context, serverConfiguration, signalingCallback.release());
    return JSValue::encode(CREATE_DOM_WRAPPER(exec, jsConstructor->globalObject(), DeprecatedPeerConnection, peerConnection.get()));
}
