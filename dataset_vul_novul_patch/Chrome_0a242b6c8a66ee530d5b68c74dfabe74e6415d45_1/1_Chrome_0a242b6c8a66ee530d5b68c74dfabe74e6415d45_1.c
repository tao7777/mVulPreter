bool PrivateScriptRunner::runDOMAttributeSetter(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder, v8::Local<v8::Value> v8Value)
{
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> descriptor;
    if (!classObject->GetOwnPropertyDescriptor(scriptState->context(), v8String(isolate, attributeName)).ToLocal(&descriptor) || !descriptor->IsObject()) {
        fprintf(stderr, "Private script error: Target DOM attribute setter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_NOTREACHED();
    }
    v8::Local<v8::Value> setter;
    if (!v8::Local<v8::Object>::Cast(descriptor)->Get(scriptState->context(), v8String(isolate, "set")).ToLocal(&setter) || !setter->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM attribute setter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_NOTREACHED();
    }
    initializeHolderIfNeeded(scriptState, classObject, holder);
     v8::Local<v8::Value> argv[] = { v8Value };
     v8::TryCatch block(isolate);
     v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callFunction(v8::Local<v8::Function>::Cast(setter), scriptState->getExecutionContext(), holder, WTF_ARRAY_LENGTH(argv), argv, isolate).ToLocal(&result)) {
         rethrowExceptionInPrivateScript(isolate, block, scriptStateInUserScript, ExceptionState::SetterContext, attributeName, className);
         block.ReThrow();
         return false;
    }
    return true;
}
