void ScriptLoader::executeScript(const ScriptSourceCode& sourceCode)
{
    ASSERT(m_alreadyStarted);

    if (sourceCode.isEmpty())
        return;

    RefPtr<Document> elementDocument(m_element->document());
    RefPtr<Document> contextDocument = elementDocument->contextDocument().get();
    if (!contextDocument)
        return;

    LocalFrame* frame = contextDocument->frame();

    bool shouldBypassMainWorldContentSecurityPolicy = (frame && frame->script().shouldBypassMainWorldContentSecurityPolicy()) || elementDocument->contentSecurityPolicy()->allowScriptNonce(m_element->fastGetAttribute(HTMLNames::nonceAttr)) || elementDocument->contentSecurityPolicy()->allowScriptHash(sourceCode.source());

     if (!m_isExternalScript && (!shouldBypassMainWorldContentSecurityPolicy && !elementDocument->contentSecurityPolicy()->allowInlineScript(elementDocument->url(), m_startLineNumber)))
         return;
 
    if (m_isExternalScript && m_resource && !m_resource->mimeTypeAllowedByNosniff()) {
        contextDocument->addConsoleMessage(SecurityMessageSource, ErrorMessageLevel, "Refused to execute script from '" + m_resource->url().elidedString() + "' because its MIME type ('" + m_resource->mimeType() + "') is not executable, and strict MIME type checking is enabled.");
        return;
     }
 
     if (frame) {
        const bool isImportedScript = contextDocument != elementDocument;
        IgnoreDestructiveWriteCountIncrementer ignoreDestructiveWriteCountIncrementer(m_isExternalScript || isImportedScript ? contextDocument.get() : 0);

        if (isHTMLScriptLoader(m_element))
            contextDocument->pushCurrentScript(toHTMLScriptElement(m_element));

        AccessControlStatus corsCheck = NotSharableCrossOrigin;
        if (sourceCode.resource() && sourceCode.resource()->passesAccessControlCheck(m_element->document().securityOrigin()))
            corsCheck = SharableCrossOrigin;

        frame->script().executeScriptInMainWorld(sourceCode, corsCheck);

        if (isHTMLScriptLoader(m_element)) {
            ASSERT(contextDocument->currentScript() == m_element);
            contextDocument->popCurrentScript();
        }
    }
}
