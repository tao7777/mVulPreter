void Document::open()
{
    ASSERT(!importLoader());

    if (m_frame) {
        if (ScriptableDocumentParser* parser = scriptableDocumentParser()) {
            if (parser->isParsing()) {
                if (parser->isExecutingScript())
                    return;

                if (!parser->wasCreatedByScript() && parser->hasInsertionPoint())
                    return;
            }
        }

        if (m_frame->loader().provisionalDocumentLoader())
            m_frame->loader().stopAllLoaders();
    }

    removeAllEventListenersRecursively();
    implicitOpen(ForceSynchronousParsing);
    if (ScriptableDocumentParser* parser = scriptableDocumentParser())
        parser->setWasCreatedByScript(true);
 
     if (m_frame)
         m_frame->loader().didExplicitOpen();
    if (m_loadEventProgress != LoadEventInProgress && pageDismissalEventBeingDispatched() == NoDismissal)
         m_loadEventProgress = LoadEventNotRun;
 }
