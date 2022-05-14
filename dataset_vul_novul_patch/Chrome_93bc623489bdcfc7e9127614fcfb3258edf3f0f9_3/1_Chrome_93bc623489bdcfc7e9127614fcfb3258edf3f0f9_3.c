v8::MaybeLocal<v8::Value> V8Debugger::functionScopes(v8::Local<v8::Function> function)
 {
     if (!enabled()) {
         NOTREACHED();
         return v8::Local<v8::Value>::New(m_isolate, v8::Undefined(m_isolate));
     }
     v8::Local<v8::Value> argv[] = { function };
     v8::Local<v8::Value> scopesValue;
    if (!callDebuggerMethod("getFunctionScopes", 1, argv).ToLocal(&scopesValue) || !scopesValue->IsArray())
         return v8::MaybeLocal<v8::Value>();
    v8::Local<v8::Array> scopes = scopesValue.As<v8::Array>();
    v8::Local<v8::Context> context = m_debuggerContext.Get(m_isolate);
    if (!markAsInternal(context, scopes, V8InternalValueType::kScopeList))
         return v8::MaybeLocal<v8::Value>();
    if (!markArrayEntriesAsInternal(context, scopes, V8InternalValueType::kScope))
         return v8::MaybeLocal<v8::Value>();
    if (!scopes->SetPrototype(context, v8::Null(m_isolate)).FromMaybe(false))
        return v8::Undefined(m_isolate);
    return scopes;
 }
