void HTMLScriptRunner::executePendingScriptAndDispatchEvent(PendingScript& pendingScript, PendingScript::Type pendingScriptType)
{
    bool errorOccurred = false;
    double loadFinishTime = pendingScript.resource() && pendingScript.resource()->url().protocolIsInHTTPFamily() ? pendingScript.resource()->loadFinishTime() : 0;
    ScriptSourceCode sourceCode = pendingScript.getSource(documentURLForScriptExecution(m_document), errorOccurred);

     pendingScript.stopWatchingForLoad(this);
 
     if (!isExecutingScript()) {
        Microtask::performCheckpoint();
         if (pendingScriptType == PendingScript::ParsingBlocking) {
             m_hasScriptsWaitingForResources = !m_document->isScriptExecutionReady();
            if (m_hasScriptsWaitingForResources)
                return;
        }
    }

    RefPtrWillBeRawPtr<Element> element = pendingScript.releaseElementAndClear();
    double compilationFinishTime = 0;
    if (ScriptLoader* scriptLoader = toScriptLoaderIfPossible(element.get())) {
        NestingLevelIncrementer nestingLevelIncrementer(m_scriptNestingLevel);
        IgnoreDestructiveWriteCountIncrementer ignoreDestructiveWriteCountIncrementer(m_document);
        if (errorOccurred)
            scriptLoader->dispatchErrorEvent();
        else {
            ASSERT(isExecutingScript());
            if (!scriptLoader->executeScript(sourceCode, &compilationFinishTime)) {
                scriptLoader->dispatchErrorEvent();
            } else {
                element->dispatchEvent(createScriptLoadEvent());
            }
        }
    }
    const double epsilon = 1;
    if (pendingScriptType == PendingScript::ParsingBlocking && !m_parserBlockingScriptAlreadyLoaded && compilationFinishTime > epsilon && loadFinishTime > epsilon) {
        Platform::current()->histogramCustomCounts("WebCore.Scripts.ParsingBlocking.TimeBetweenLoadedAndCompiled", (compilationFinishTime - loadFinishTime) * 1000, 0, 10000, 50);
    }

    ASSERT(!isExecutingScript());
}
