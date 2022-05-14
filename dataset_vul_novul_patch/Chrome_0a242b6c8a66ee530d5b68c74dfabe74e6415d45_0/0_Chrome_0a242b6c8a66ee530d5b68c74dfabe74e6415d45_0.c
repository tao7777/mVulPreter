v8::Local<v8::Value> PrivateScriptRunner::runDOMAttributeGetter(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder)
{
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> descriptor;
    if (!classObject->GetOwnPropertyDescriptor(scriptState->context(), v8String(isolate, attributeName)).ToLocal(&descriptor) || !descriptor->IsObject()) {
        fprintf(stderr, "Private script error: Target DOM attribute getter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_NOTREACHED();
    }
    v8::Local<v8::Value> getter;
    if (!v8::Local<v8::Object>::Cast(descriptor)->Get(scriptState->context(), v8String(isolate, "get")).ToLocal(&getter) || !getter->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM attribute getter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_NOTREACHED();
    }
     initializeHolderIfNeeded(scriptState, classObject, holder);
     v8::TryCatch block(isolate);
     v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callInternalFunction(v8::Local<v8::Function>::Cast(getter), holder, 0, 0, isolate).ToLocal(&result)) {
         rethrowExceptionInPrivateScript(isolate, block, scriptStateInUserScript, ExceptionState::GetterContext, attributeName, className);
         block.ReThrow();
         return v8::Local<v8::Value>();
    }
    return result;
}
