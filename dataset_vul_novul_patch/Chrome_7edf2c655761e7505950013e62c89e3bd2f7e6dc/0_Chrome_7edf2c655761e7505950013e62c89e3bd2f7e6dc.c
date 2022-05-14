ScriptValue ScriptController::executeScriptInMainWorld(const ScriptSourceCode& sourceCode, AccessControlStatus corsStatus)
{
    String sourceURL = sourceCode.url();
    const String* savedSourceURL = m_sourceURL;
    m_sourceURL = &sourceURL;

    v8::HandleScope handleScope;
    v8::Handle<v8::Context> v8Context = ScriptController::mainWorldContext(m_frame);
     if (v8Context.IsEmpty())
         return ScriptValue();
 
     RefPtr<Frame> protect(m_frame);
    if (m_frame->loader()->stateMachine()->isDisplayingInitialEmptyDocument())
        m_frame->loader()->didAccessInitialDocument();

    v8::Context::Scope scope(v8Context);
     v8::Local<v8::Value> object = compileAndRunScript(sourceCode, corsStatus);
 
     m_sourceURL = savedSourceURL;

    if (object.IsEmpty())
        return ScriptValue();

    return ScriptValue(object);
}
