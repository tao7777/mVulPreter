v8::Local<v8::Value> V8Debugger::generatorObjectLocation(v8::Local<v8::Object> object)
v8::Local<v8::Value> V8Debugger::generatorObjectLocation(v8::Local<v8::Context> context, v8::Local<v8::Object> object)
 {
     if (!enabled()) {
         NOTREACHED();
         return v8::Null(m_isolate);
     }
     v8::Local<v8::Value> argv[] = { object };
     v8::Local<v8::Value> location = callDebuggerMethod("getGeneratorObjectLocation", 1, argv).ToLocalChecked();
    v8::Local<v8::Value> copied;
    if (!copyValueFromDebuggerContext(m_isolate, debuggerContext(), context, location).ToLocal(&copied) || !copied->IsObject())
         return v8::Null(m_isolate);
    if (!markAsInternal(context, v8::Local<v8::Object>::Cast(copied), V8InternalValueType::kLocation))
         return v8::Null(m_isolate);
    return copied;
 }
