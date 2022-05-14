v8::MaybeLocal<v8::Value> V8Debugger::functionScopes(v8::Local<v8::Function> function)
v8::MaybeLocal<v8::Value> V8Debugger::functionScopes(v8::Local<v8::Context> context, v8::Local<v8::Function> function)
 {
     if (!enabled()) {
         NOTREACHED();
         return v8::Local<v8::Value>::New(m_isolate, v8::Undefined(m_isolate));
     }
     v8::Local<v8::Value> argv[] = { function };
     v8::Local<v8::Value> scopesValue;
    if (!callDebuggerMethod("getFunctionScopes", 1, argv).ToLocal(&scopesValue))
         return v8::MaybeLocal<v8::Value>();
    v8::Local<v8::Value> copied;
    if (!copyValueFromDebuggerContext(m_isolate, debuggerContext(), context, scopesValue).ToLocal(&copied) || !copied->IsArray())
         return v8::MaybeLocal<v8::Value>();
    if (!markAsInternal(context, v8::Local<v8::Array>::Cast(copied), V8InternalValueType::kScopeList))
         return v8::MaybeLocal<v8::Value>();
    if (!markArrayEntriesAsInternal(context, v8::Local<v8::Array>::Cast(copied), V8InternalValueType::kScope))
        return v8::MaybeLocal<v8::Value>();
    return copied;
 }
