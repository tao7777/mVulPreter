 v8::Local<v8::Value> V8Debugger::functionLocation(v8::Local<v8::Context> context, v8::Local<v8::Function> function)
{
    int scriptId = function->ScriptId();
    if (scriptId == v8::UnboundScript::kNoScriptId)
        return v8::Null(m_isolate);
    int lineNumber = function->GetScriptLineNumber();
    int columnNumber = function->GetScriptColumnNumber();
     if (lineNumber == v8::Function::kLineOffsetNotFound || columnNumber == v8::Function::kLineOffsetNotFound)
         return v8::Null(m_isolate);
     v8::Local<v8::Object> location = v8::Object::New(m_isolate);
     if (!location->Set(context, toV8StringInternalized(m_isolate, "scriptId"), toV8String(m_isolate, String16::fromInteger(scriptId))).FromMaybe(false))
         return v8::Null(m_isolate);
     if (!location->Set(context, toV8StringInternalized(m_isolate, "lineNumber"), v8::Integer::New(m_isolate, lineNumber)).FromMaybe(false))
        return v8::Null(m_isolate);
    if (!location->Set(context, toV8StringInternalized(m_isolate, "columnNumber"), v8::Integer::New(m_isolate, columnNumber)).FromMaybe(false))
        return v8::Null(m_isolate);
    if (!markAsInternal(context, location, V8InternalValueType::kLocation))
        return v8::Null(m_isolate);
    return location;
}
