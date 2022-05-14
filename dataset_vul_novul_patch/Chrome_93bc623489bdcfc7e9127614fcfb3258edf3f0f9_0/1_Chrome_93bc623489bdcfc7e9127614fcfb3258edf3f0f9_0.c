v8::Local<v8::Object> V8Console::createConsole(InspectedContext* inspectedContext, bool hasMemoryAttribute)
{
    v8::Local<v8::Context> context = inspectedContext->context();
    v8::Context::Scope contextScope(context);
    v8::Isolate* isolate = context->GetIsolate();
     v8::MicrotasksScope microtasksScope(isolate, v8::MicrotasksScope::kDoNotRunMicrotasks);
 
     v8::Local<v8::Object> console = v8::Object::New(isolate);
 
     createBoundFunctionProperty(context, console, "debug", V8Console::debugCallback);
     createBoundFunctionProperty(context, console, "error", V8Console::errorCallback);
    createBoundFunctionProperty(context, console, "info", V8Console::infoCallback);
    createBoundFunctionProperty(context, console, "log", V8Console::logCallback);
    createBoundFunctionProperty(context, console, "warn", V8Console::warnCallback);
    createBoundFunctionProperty(context, console, "dir", V8Console::dirCallback);
    createBoundFunctionProperty(context, console, "dirxml", V8Console::dirxmlCallback);
    createBoundFunctionProperty(context, console, "table", V8Console::tableCallback);
    createBoundFunctionProperty(context, console, "trace", V8Console::traceCallback);
    createBoundFunctionProperty(context, console, "group", V8Console::groupCallback);
    createBoundFunctionProperty(context, console, "groupCollapsed", V8Console::groupCollapsedCallback);
    createBoundFunctionProperty(context, console, "groupEnd", V8Console::groupEndCallback);
    createBoundFunctionProperty(context, console, "clear", V8Console::clearCallback);
    createBoundFunctionProperty(context, console, "count", V8Console::countCallback);
    createBoundFunctionProperty(context, console, "assert", V8Console::assertCallback);
    createBoundFunctionProperty(context, console, "markTimeline", V8Console::markTimelineCallback);
    createBoundFunctionProperty(context, console, "profile", V8Console::profileCallback);
    createBoundFunctionProperty(context, console, "profileEnd", V8Console::profileEndCallback);
    createBoundFunctionProperty(context, console, "timeline", V8Console::timelineCallback);
    createBoundFunctionProperty(context, console, "timelineEnd", V8Console::timelineEndCallback);
    createBoundFunctionProperty(context, console, "time", V8Console::timeCallback);
     createBoundFunctionProperty(context, console, "timeEnd", V8Console::timeEndCallback);
     createBoundFunctionProperty(context, console, "timeStamp", V8Console::timeStampCallback);
 
    bool success = console->SetPrototype(context, v8::Object::New(isolate)).FromMaybe(false);
    DCHECK(success);
     if (hasMemoryAttribute)
         console->SetAccessorProperty(toV8StringInternalized(isolate, "memory"), V8_FUNCTION_NEW_REMOVE_PROTOTYPE(context, V8Console::memoryGetterCallback, console, 0).ToLocalChecked(), V8_FUNCTION_NEW_REMOVE_PROTOTYPE(context, V8Console::memorySetterCallback, v8::Local<v8::Value>(), 0).ToLocalChecked(), static_cast<v8::PropertyAttribute>(v8::None), v8::DEFAULT);
 
    console->SetPrivate(context, inspectedContextPrivateKey(isolate), v8::External::New(isolate, inspectedContext));
    return console;
}
