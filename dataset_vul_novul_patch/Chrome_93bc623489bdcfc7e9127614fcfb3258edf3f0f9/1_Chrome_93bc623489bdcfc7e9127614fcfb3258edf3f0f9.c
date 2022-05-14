v8::Local<v8::Object> V8Console::createCommandLineAPI(InspectedContext* inspectedContext)
{
    v8::Local<v8::Context> context = inspectedContext->context();
    v8::Isolate* isolate = context->GetIsolate();
     v8::MicrotasksScope microtasksScope(isolate, v8::MicrotasksScope::kDoNotRunMicrotasks);
 
     v8::Local<v8::Object> commandLineAPI = v8::Object::New(isolate);
 
     createBoundFunctionProperty(context, commandLineAPI, "dir", V8Console::dirCallback, "function dir(value) { [Command Line API] }");
     createBoundFunctionProperty(context, commandLineAPI, "dirxml", V8Console::dirxmlCallback, "function dirxml(value) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "profile", V8Console::profileCallback, "function profile(title) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "profileEnd", V8Console::profileEndCallback, "function profileEnd(title) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "clear", V8Console::clearCallback, "function clear() { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "table", V8Console::tableCallback, "function table(data, [columns]) { [Command Line API] }");

    createBoundFunctionProperty(context, commandLineAPI, "keys", V8Console::keysCallback, "function keys(object) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "values", V8Console::valuesCallback, "function values(object) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "debug", V8Console::debugFunctionCallback, "function debug(function) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "undebug", V8Console::undebugFunctionCallback, "function undebug(function) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "monitor", V8Console::monitorFunctionCallback, "function monitor(function) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "unmonitor", V8Console::unmonitorFunctionCallback, "function unmonitor(function) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "inspect", V8Console::inspectCallback, "function inspect(object) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "copy", V8Console::copyCallback, "function copy(value) { [Command Line API] }");
    createBoundFunctionProperty(context, commandLineAPI, "$_", V8Console::lastEvaluationResultCallback);
    createBoundFunctionProperty(context, commandLineAPI, "$0", V8Console::inspectedObject0);
    createBoundFunctionProperty(context, commandLineAPI, "$1", V8Console::inspectedObject1);
    createBoundFunctionProperty(context, commandLineAPI, "$2", V8Console::inspectedObject2);
    createBoundFunctionProperty(context, commandLineAPI, "$3", V8Console::inspectedObject3);
    createBoundFunctionProperty(context, commandLineAPI, "$4", V8Console::inspectedObject4);

    inspectedContext->inspector()->client()->installAdditionalCommandLineAPI(context, commandLineAPI);

    commandLineAPI->SetPrivate(context, inspectedContextPrivateKey(isolate), v8::External::New(isolate, inspectedContext));
    return commandLineAPI;
}
