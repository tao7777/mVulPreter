 void WasmCompileStreamingImpl(const v8::FunctionCallbackInfo<v8::Value>& args) {
   v8::Isolate* isolate = args.GetIsolate();
  ScriptState* script_state = ScriptState::ForCurrentRealm(args);
 
   v8::Local<v8::Function> compile_callback =
       v8::Function::New(isolate, CompileFromResponseCallback);


  V8SetReturnValue(args, ScriptPromise::Cast(script_state, args[0])
                             .Then(compile_callback)
                             .V8Value());

}
