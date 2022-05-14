bool EditorClientBlackBerry::shouldChangeSelectedRange(Range* fromRange, Range* toRange, EAffinity affinity, bool stillSelecting)
{
    if (m_webPagePrivate->m_dumpRenderTree)
        return m_webPagePrivate->m_dumpRenderTree->shouldChangeSelectedDOMRangeToDOMRangeAffinityStillSelecting(fromRange, toRange, static_cast<int>(affinity), stillSelecting);
 
     Frame* frame = m_webPagePrivate->focusedOrMainFrame();
     if (frame && frame->document()) {
        if (Node* focusedNode = frame->document()->focusedNode()) {
            if (focusedNode->hasTagName(HTMLNames::selectTag))
                return false;
            if (focusedNode->isElementNode() && DOMSupport::isPopupInputField(static_cast<Element*>(focusedNode)))
                return false;
        }
 
         if (m_webPagePrivate->m_inputHandler->isInputMode() && fromRange && toRange && (fromRange->startContainer() == toRange->startContainer()))
            m_webPagePrivate->m_inputHandler->notifyClientOfKeyboardVisibilityChange(true);
    }

    return true;
}
