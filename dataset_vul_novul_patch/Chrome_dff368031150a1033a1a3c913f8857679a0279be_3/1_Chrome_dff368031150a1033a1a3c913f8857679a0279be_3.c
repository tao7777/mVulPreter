void HTMLScriptRunner::runScript(Element* script, const TextPosition& scriptStartPosition)
{
    ASSERT(m_document);
    ASSERT(!hasParserBlockingScript());
    {
        ScriptLoader* scriptLoader = toScriptLoaderIfPossible(script);

        ASSERT(scriptLoader);
        if (!scriptLoader)
            return;

         ASSERT(scriptLoader->isParserInserted());
 
         if (!isExecutingScript())
            Microtask::performCheckpoint();
 
         InsertionPointRecord insertionPointRecord(m_host->inputStream());
         NestingLevelIncrementer nestingLevelIncrementer(m_scriptNestingLevel);

        scriptLoader->prepareScript(scriptStartPosition);

        if (!scriptLoader->willBeParserExecuted())
            return;

        if (scriptLoader->willExecuteWhenDocumentFinishedParsing()) {
            requestDeferredScript(script);
        } else if (scriptLoader->readyToBeParserExecuted()) {
            if (m_scriptNestingLevel == 1) {
                m_parserBlockingScript.setElement(script);
                m_parserBlockingScript.setStartingPosition(scriptStartPosition);
            } else {
                ScriptSourceCode sourceCode(script->textContent(), documentURLForScriptExecution(m_document), scriptStartPosition);
                scriptLoader->executeScript(sourceCode);
            }
        } else {
            requestParsingBlockingScript(script);
        }
    }
}
