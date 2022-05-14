bool InputHandler::shouldRequestSpellCheckingOptionsForPoint(Platform::IntPoint& point, const Element* touchedElement, imf_sp_text_t& spellCheckingOptionRequest)
 {
     if (!isActiveTextEdit())
         return false;

    Element* currentFocusElement = m_currentFocusElement.get();
    if (!currentFocusElement || !currentFocusElement->isElementNode())
        return false;

    while (!currentFocusElement->isRootEditableElement()) {
        Element* parentElement = currentFocusElement->parentElement();
        if (!parentElement)
            break;
        currentFocusElement = parentElement;
    }

     if (touchedElement != currentFocusElement)
         return false;
 
    LayoutPoint contentPos(m_webPage->mapFromViewportToContents(point));
     contentPos = DOMSupport::convertPointToFrame(m_webPage->mainFrame(), m_webPage->focusedOrMainFrame(), roundedIntPoint(contentPos));
 
     Document* document = currentFocusElement->document();
    ASSERT(document);

    RenderedDocumentMarker* marker = document->markers()->renderedMarkerContainingPoint(contentPos, DocumentMarker::Spelling);
    if (!marker)
        return false;

    m_didSpellCheckWord = true;

    spellCheckingOptionRequest.startTextPosition = marker->startOffset();
    spellCheckingOptionRequest.endTextPosition = marker->endOffset();

    m_spellCheckingOptionsRequest.startTextPosition = 0;
    m_spellCheckingOptionsRequest.endTextPosition = 0;

    SpellingLog(LogLevelInfo, "InputHandler::shouldRequestSpellCheckingOptionsForPoint Found spelling marker at point %d, %d\nMarker start %d end %d",
        point.x(), point.y(), spellCheckingOptionRequest.startTextPosition, spellCheckingOptionRequest.endTextPosition);

    return true;
}
