 void HTMLLinkElement::insertedIntoDocument()
 {
     HTMLElement::insertedIntoDocument();
     document()->addStyleSheetCandidateNode(this, m_createdByParser);
 
    if (document()->hasListenerType(Document::BEFORELOAD_LISTENER)) {
        m_shouldProcessAfterAttach = true;
        return;
    }
     process();
 }
