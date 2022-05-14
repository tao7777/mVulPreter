static v8::Local<v8::Value> compileAndRunPrivateScript(ScriptState* scriptState,
                                                       String scriptClassName,
                                                       const char* source,
                                                       size_t size) {
  v8::Isolate* isolate = scriptState->isolate();
  v8::TryCatch block(isolate);
  String sourceString(source, size);
  String fileName = scriptClassName + ".js";
 
   v8::Local<v8::Context> context = scriptState->context();
   v8::Local<v8::Object> global = context->Global();
  v8::Local<v8::String> key = v8String(isolate, "privateScriptController");

  if (global->HasOwnProperty(context, key).ToChecked()) {
    v8::Local<v8::Value> privateScriptController =
        global->Get(context, key).ToLocalChecked();
    CHECK(privateScriptController->IsObject());
     v8::Local<v8::Object> privateScriptControllerObject =
         privateScriptController.As<v8::Object>();
     v8::Local<v8::Value> importFunctionValue =
        privateScriptControllerObject->Get(context, v8String(isolate, "import"))
            .ToLocalChecked();
    if (importFunctionValue->IsUndefined()) {
      v8::Local<v8::Function> function;
      if (!v8::FunctionTemplate::New(isolate, importFunction)
               ->GetFunction(context)
               .ToLocal(&function) ||
          !v8CallBoolean(privateScriptControllerObject->Set(
              context, v8String(isolate, "import"), function))) {
        dumpV8Message(context, block.Message());
        LOG(FATAL)
            << "Private script error: Setting import function failed. (Class "
               "name = "
            << scriptClassName.utf8().data() << ")";
      }
    }
  }

  v8::Local<v8::Script> script;
  if (!v8Call(V8ScriptRunner::compileScript(
                  v8String(isolate, sourceString), fileName, String(),
                  TextPosition::minimumPosition(), isolate, nullptr, nullptr,
                  nullptr, NotSharableCrossOrigin),
              script, block)) {
    dumpV8Message(context, block.Message());
    LOG(FATAL) << "Private script error: Compile failed. (Class name = "
               << scriptClassName.utf8().data() << ")";
  }

  v8::Local<v8::Value> result;
  if (!v8Call(V8ScriptRunner::runCompiledInternalScript(isolate, script),
              result, block)) {
    dumpV8Message(context, block.Message());
    LOG(FATAL) << "Private script error: installClass() failed. (Class name = "
               << scriptClassName.utf8().data() << ")";
  }
  return result;
}
