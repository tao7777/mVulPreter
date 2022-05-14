void V8WindowShell::namedItemRemoved(HTMLDocument* document, const AtomicString& name)
{
    ASSERT(m_world->isMainWorld());

     if (m_context.isEmpty())
         return;
 
    if (document->hasNamedItem(name.impl()) || document->hasExtraNamedItem(name.impl()))
         return;
 
     v8::HandleScope handleScope(m_isolate);
    v8::Context::Scope contextScope(m_context.newLocal(m_isolate));

    ASSERT(!m_document.isEmpty());
    v8::Handle<v8::Object> documentHandle = m_document.newLocal(m_isolate);
    checkDocumentWrapper(documentHandle, document);
    documentHandle->Delete(v8String(name, m_isolate));
}
