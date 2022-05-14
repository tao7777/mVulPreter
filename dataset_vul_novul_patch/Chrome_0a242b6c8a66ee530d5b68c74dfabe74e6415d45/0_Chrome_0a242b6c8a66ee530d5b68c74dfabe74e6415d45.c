static void initializeHolderIfNeeded(ScriptState* scriptState, v8::Local<v8::Object> classObject, v8::Local<v8::Value> holder)
{
    RELEASE_ASSERT(!holder.IsEmpty());
    RELEASE_ASSERT(holder->IsObject());
    v8::Local<v8::Object> holderObject = v8::Local<v8::Object>::Cast(holder);
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Context> context = scriptState->context();
    auto privateIsInitialized = V8PrivateProperty::getPrivateScriptRunnerIsInitialized(isolate);
    if (privateIsInitialized.hasValue(context, holderObject))
        return; // Already initialized.

    v8::TryCatch block(isolate);
    v8::Local<v8::Value> initializeFunction;
     if (classObject->Get(scriptState->context(), v8String(isolate, "initialize")).ToLocal(&initializeFunction) && initializeFunction->IsFunction()) {
         v8::TryCatch block(isolate);
         v8::Local<v8::Value> result;
        if (!V8ScriptRunner::callInternalFunction(v8::Local<v8::Function>::Cast(initializeFunction), holder, 0, 0, isolate).ToLocal(&result)) {
             fprintf(stderr, "Private script error: Object constructor threw an exception.\n");
             dumpV8Message(context, block.Message());
             RELEASE_NOTREACHED();
        }
    }

    if (classObject->GetPrototype() != holderObject->GetPrototype()) {
        if (!v8CallBoolean(classObject->SetPrototype(context, holderObject->GetPrototype()))) {
            fprintf(stderr, "Private script error: SetPrototype failed.\n");
            dumpV8Message(context, block.Message());
            RELEASE_NOTREACHED();
        }
    }
    if (!v8CallBoolean(holderObject->SetPrototype(context, classObject))) {
        fprintf(stderr, "Private script error: SetPrototype failed.\n");
        dumpV8Message(context, block.Message());
        RELEASE_NOTREACHED();
    }

    privateIsInitialized.set(context, holderObject, v8Boolean(true, isolate));
}
