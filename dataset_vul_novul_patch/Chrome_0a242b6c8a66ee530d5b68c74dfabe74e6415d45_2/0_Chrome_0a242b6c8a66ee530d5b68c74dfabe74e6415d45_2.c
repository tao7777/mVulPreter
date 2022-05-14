v8::Local<v8::Value> PrivateScriptRunner::runDOMMethod(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* methodName, v8::Local<v8::Value> holder, int argc, v8::Local<v8::Value> argv[])
{
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> method;
    if (!classObject->Get(scriptState->context(), v8String(scriptState->isolate(), methodName)).ToLocal(&method) || !method->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM method was not found. (Class name = %s, Method name = %s)\n", className, methodName);
        RELEASE_NOTREACHED();
    }
     initializeHolderIfNeeded(scriptState, classObject, holder);
     v8::TryCatch block(scriptState->isolate());
     v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callInternalFunction(v8::Local<v8::Function>::Cast(method), holder, argc, argv, scriptState->isolate()).ToLocal(&result)) {
         rethrowExceptionInPrivateScript(scriptState->isolate(), block, scriptStateInUserScript, ExceptionState::ExecutionContext, methodName, className);
         block.ReThrow();
         return v8::Local<v8::Value>();
    }
    return result;
}
